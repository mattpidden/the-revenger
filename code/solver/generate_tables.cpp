#include <vector>
#include <limits>
#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>
#include <cmath>
#include <fstream>
#include <unordered_map>
#include <queue>
#include <functional>
#include <cassert>
#include "cube.h"
#include <omp.h>
#include "BooPHF.h"

using namespace boomphf;

// Save / load depths
void save_depths(const std::vector<int> &depths, const std::string &filename) {
    std::ofstream out(filename, std::ios::binary);
    uint64_t sz = depths.size();
    out.write(reinterpret_cast<const char*>(&sz), sizeof(sz));
    out.write(reinterpret_cast<const char*>(depths.data()), sizeof(int)*sz);
}



void build_and_save_mph(const std::unordered_map<std::string,int> &table, const std::string &mph_filename, const std::string &depth_filename) {
    std::vector<std::string> keys;
    keys.reserve(table.size());
    for (auto &kv : table) {
        keys.push_back(kv.first);
    }
    auto mph = new boomphf::mphf<std::string, MyStringHash>(
        keys.size(),   // # of keys
        keys,          // pass the vector
        1,             // # of threads
        4.0  ,         // gamma
        false,         // writeEach
        false,         // progress
        0.03f          // alpha ratio
    );
    // Build parallel depths
    std::vector<int> depths(keys.size(), -1);
    for (auto &kv : table) {
        uint64_t idx = mph->lookup(kv.first);
        depths[idx] = kv.second;
    }
    // Save depths and hash
    save_depths(depths, depth_filename);
    std::ofstream ofs(mph_filename, std::ios::binary);
    mph->save(ofs);

    delete mph;
    std::cout << "BBHash MPH built & saved to " << mph_filename << " and " << depth_filename << "\n" << std::endl;
}

std::vector<Cube4x4> generate_cube_table(const Phase &phase, const std::vector<Cube4x4> solved_cubes, const std::vector<Move>& moveset, int depth_limit = -1) {
    std::unordered_map<std::string, int> table;
    std::vector<Cube4x4> cube_table;
    std::queue<Cube4x4> frontier;
    
    for (Cube4x4 cube : solved_cubes) {
        std::string state = phase.mask(cube);
        table[state] = 0;
        cube_table.push_back(cube);
        frontier.push(cube);
    }
    
    int max_depth = 0;
    while (!frontier.empty()) {
        Cube4x4 current_cube = frontier.front();
        frontier.pop();
        std::string current_state = phase.mask(current_cube);
        int depth = table[current_state];
        
        if (depth > max_depth) {
            max_depth = depth;
            std::cout << "Reached depth " << max_depth << " | States so far: " << table.size() << std::endl;
        }
        
        if (depth_limit < 0 || depth < depth_limit) {
            for (const auto& move : moveset) {
                Cube4x4 next_cube = current_cube; 
                next_cube.move(move);
                std::string next_state = phase.mask(next_cube);
                
                if (table.find(next_state) == table.end()) {
                    table[next_state] = depth + 1;
                    cube_table.push_back(next_cube);
                    frontier.push(next_cube);
                }
            }
        }
    }
    
    std::cout << "Final depth reached: " << max_depth << std::endl;
    std::cout << "Total states found: " << table.size() << std::endl;
    std::cout << "Total cubes found: " << cube_table.size() << std::endl;
    
    return cube_table;
}

std::unordered_map<std::string, int> generate_table(const Phase &phase, const std::vector<Cube4x4> solved_cubes, const std::vector<Move>& moveset, int depth_limit = -1) {
    std::unordered_map<std::string, int> table;
    std::queue<Cube4x4> frontier;
    
    for (Cube4x4 cube : solved_cubes) {
        std::string state = phase.mask(cube);
        table[state] = 0;
        frontier.push(cube);
    }
    
    int max_depth = 0;
    while (!frontier.empty()) {
        Cube4x4 current_cube = frontier.front();
        frontier.pop();
        std::string current_state = phase.mask(current_cube);
        int depth = table[current_state];
        
        if (depth > max_depth) {
            max_depth = depth;
            //std::string mph_file = phase.table_filename + ".mph";
            //std::string depth_file = phase.table_filename + ".depths";
            //build_and_save_mph(table, mph_file, depth_file);
            std::cout << "Reached depth " << max_depth << " | States so far: " << table.size() << std::endl;

        }
        
        if (depth_limit < 0 || depth < depth_limit) {
            for (const auto& move : moveset) {
                Cube4x4 next_cube = current_cube; 
                next_cube.move(move);
                std::string next_state = phase.mask(next_cube);
                
                if (table.find(next_state) == table.end()) {
                    table[next_state] = depth + 1;
                    frontier.push(next_cube);
                }
            }
        }
    }
    
    std::cout << "Final depth reached: " << max_depth << std::endl;
    std::cout << "Total states found: " << table.size() << std::endl;
    
    return table;
}


std::unordered_map<std::string, int> generate_table_layered_omp(const Phase &phase, const std::vector<Cube4x4> &start_cubes, const std::vector<Move> &moveset, int depth_limit = -1) {
    // This map keeps track of (mask_string -> BFS depth).
    std::unordered_map<std::string,int> visited;
    visited.reserve(1000000000); // optional: give some hint

    // The current frontier for BFS expansions (layer d)
    std::vector<Cube4x4> current_layer;

    // Initialize with solved states
    for (Cube4x4 cube : start_cubes) {
        std::string state = phase.mask(cube);
        visited[state] = 0;
        current_layer.push_back(cube);
    }

    int depth = 0;
    while (!current_layer.empty() && (depth_limit < 0 || depth < depth_limit)) {
        // We'll build the next layer (depth d+1)
        // We do it by collecting newly discovered states in parallel,
        // then merging them into a single vector after.
        std::vector<Cube4x4> next_layer;
        next_layer.reserve(current_layer.size() * moveset.size()); // rough guess

        // Each thread has a local container of <new_mask, new_cube>
        // so there's no contention writing to shared memory.
        // We'll merge at the end of the parallel region.
        #pragma omp parallel
        {
            std::vector<std::pair<std::string, Cube4x4>> local_new;
            local_new.reserve(128); // arbitrary

            #pragma omp for schedule(dynamic)
            for (int i = 0; i < (int)current_layer.size(); i++) {
                Cube4x4 parent_cube = current_layer[i];
                int parent_depth = 0; // we’ll retrieve it from visited

                // The parent_mask is needed to read parent_depth
                // But looking it up in a map is O(logN) or average O(1). 
                // We'll do it once outside the moves loop.
                std::string parent_mask = phase.mask(parent_cube);
                auto it = visited.find(parent_mask);
                if (it == visited.end()) {
                    // This should never happen if BFS is correct
                    continue;
                }
                parent_depth = it->second;

                // If we can still go deeper
                if (depth_limit < 0 || parent_depth < depth_limit) {
                    // Generate children
                    for (auto &mv : moveset) {
                        Cube4x4 child_cube = parent_cube;
                        child_cube.move(mv);
                        std::string child_mask = phase.mask(child_cube);

                        // Check if not visited
                        // (We only do a local check here, to avoid locking visited.)
                        // We'll do the real check+insert after we leave the parallel region.
                        // But for big BFS, you probably want a better concurrency approach
                        // or a second data structure. This is the simplest illustration.
                        // We'll just store them in local_new for now.
                        // We'll do the global check in the merge phase.
                        local_new.emplace_back(child_mask, child_cube);
                    }
                }
            }

            // End of parallel for; now we want to merge local_new into visited + next_layer
            #pragma omp critical
            {
                for (auto &pair : local_new) {
                    const std::string &child_mask = pair.first;
                    Cube4x4 &child_cube = pair.second;

                    // Insert into visited if not present
                    auto it2 = visited.find(child_mask);
                    if (it2 == visited.end()) {
                        // discovered at depth = parent_depth + 1
                        // But we must know the parent's depth. 
                        // We used 'parent_depth' in the loop, so let's do a second approach:
                        // We'll assume this node is at (depth+1).
                        // Because we're layering BFS, that’s accurate.
                        visited[child_mask] = depth + 1;
                        next_layer.push_back(std::move(child_cube));
                    }
                }
            }
            // local_new is cleaned up as we exit the parallel region
        } // end omp parallel

        depth++;
        current_layer = std::move(next_layer);

        // Some printing if we want to track progress
        std::cout << "Reached depth " << depth << " | " << visited.size() << " states so far" << std::endl;
    }

    std::cout << "BFS done. Final depth reached: " << depth << " | Total states found: " << visited.size() << std::endl;

    return visited;
}



int main() {         
    

    // phase 1,5,6,7,8 are working perfectly
    // phase 2 mask needs tweaking to make smaller state space
    // phase 3,4 masks take a long time to compute

    // phase 2 mask does not account for parities (but it might)
    // phase 4 mask does account for parities (but it might)

    // std::cout << "Generating " << phase1.name << " tables...\n";
    // Cube4x4 cube1;
    // auto table1 = generate_table(phase1, {cube1}, phase1.moves, phase1.table_depth_limit); // 735,471 at 8
    // std::string mph_file1 = phase1.table_filename + ".mph";
    // std::string depth_file1 = phase1.table_filename + ".depths";
    // build_and_save_mph(table1, mph_file1, depth_file1);

    std::cout << "Generating " << phase2.name << " tables...\n";
    Cube4x4 centre_cube;
    std::vector<Cube4x4> centre_table = generate_cube_table(phase2, {centre_cube}, {R2, L2, F, B, U, D, r2, l2, f2, b2, u2, d2}, 3); // 12  at 3
    auto table2 = generate_table_layered_omp(phase2, centre_table, phase2.moves, phase2.table_depth_limit); // 3,695,452 at 5 (limited)
    std::string mph_file2 = phase2.table_filename + ".mph";
    std::string depth_file2 = phase2.table_filename + ".depths";
    build_and_save_mph(table2, mph_file2, depth_file2);

    // std::cout << "Generating " << phase3.name << " tables...\n";
    // Cube4x4 centre_column_cube;
    // std::vector<Cube4x4> centre_column_table = generate_cube_table(phase3, {centre_column_cube}, {R2, L2, F2, B2, U, U_PRIME, U2, D, D_PRIME, D2, r2, l2, f2, b2}, 4); // 36 at 4
    // auto table3 = generate_table(phase3, centre_column_table, phase3.moves, phase3.table_depth_limit); // 215,028 at 13
    // std::string mph_file3 = phase3.table_filename + ".mph";
    // std::string depth_file3 = phase3.table_filename + ".depths";
    // build_and_save_mph(table3, mph_file3, depth_file3);

    // std::cout << "Generating " << phase4.name << " tables...\n";
    // Cube4x4 cube4;
    // auto table4 = generate_table(phase4, {cube4}, phase4.moves, phase4.table_depth_limit); // 12,089,495 at 9
    // std::string mph_file4 = phase4.table_filename + ".mph";
    // std::string depth_file4 = phase4.table_filename + ".depths";
    // build_and_save_mph(table4, mph_file4, depth_file4);

    // Cube4x4 cube5;
    // std::cout << "Generating " << phase5.name << " tables...\n";
    // auto table5 = generate_table(phase5, {cube5}, phase5.moves, phase5.table_depth_limit); // 2,048 at 8
    // std::string mph_file5 = phase5.table_filename + ".mph";
    // std::string depth_file5 = phase5.table_filename + ".depths";
    // build_and_save_mph(table5, mph_file5, depth_file5);

    // Cube4x4 cube6;
    // std::cout << "Generating " << phase6.name << " tables...\n";
    // auto table6 = generate_table(phase6, {cube6}, phase6.moves, phase6.table_depth_limit); // 1,082,565 at 11
    // std::string mph_file6 = phase6.table_filename + ".mph";
    // std::string depth_file6 = phase6.table_filename + ".depths";
    // build_and_save_mph(table6, mph_file6, depth_file6);
    
    // std::cout << "Generating " << phase7.name << " tables...\n";
    // Cube4x4 corner_cube;
    // std::vector<Cube4x4> corner_table = generate_cube_table(phase7, {corner_cube}, {R2, L2, F2, B2, U2, D2}, 5); // 96 at 4
    // auto table7 = generate_table(phase7, corner_table, phase7.moves, phase7.table_depth_limit);  // 2,822,398 at 14
    // std::string mph_file7 = phase7.table_filename + ".mph";
    // std::string depth_file7 = phase7.table_filename + ".depths";
    // build_and_save_mph(table7, mph_file7, depth_file7);

    // std::cout << "Generating " << phase8.name << " tables...\n";
    // Cube4x4 cube8;
    // auto table8 = generate_table(phase8, {cube8}, phase8.moves, phase8.table_depth_limit); // 663,552 at 15
    // std::string mph_file8 = phase8.table_filename + ".mph";
    // std::string depth_file8 = phase8.table_filename + ".depths";
    // build_and_save_mph(table8, mph_file8, depth_file8);

    return 0;
}