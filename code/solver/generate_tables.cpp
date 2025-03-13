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
        8.0  ,         // gamma
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

    size_t bloom_size_in_bits = 10000000ULL;
    int bloom_num_hashes = 7;
    BloomFilter bloom(bloom_size_in_bits, bloom_num_hashes);
    
    for (Cube4x4 cube : solved_cubes) {
        std::string state = phase.mask(cube);
        table[state] = 0;
        bloom.add(state);
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
                    bloom.add(next_state);
                    frontier.push(next_cube);
                }
            }
        }
    }
    
    std::cout << "Final depth reached: " << max_depth << std::endl;
    std::cout << "Total states found: " << table.size() << std::endl;

    std::string bloom_filename = phase.table_filename + ".bloom";
    bloom.save(bloom_filename);
    std::cout << "Saved Bloom filter to " << bloom_filename << std::endl;
    
    return table;
}

// Use this for large phases on BC4
std::unordered_map<std::string, int> generate_table_layered_omp(const Phase &phase, const std::vector<Cube4x4> &start_cubes, const std::vector<Move> &moveset, int depth_limit = -1) {
    std::unordered_map<std::string,int> visited;
    std::vector<Cube4x4> current_layer;
    size_t bloom_size_in_bits = 10000000ULL;
    int bloom_num_hashes = 7;
    BloomFilter bloom(bloom_size_in_bits, bloom_num_hashes);

    for (Cube4x4 cube : start_cubes) {
        std::string state = phase.mask(cube);
        visited[state] = 0;
        bloom.add(state);
        current_layer.push_back(cube);
    }

    int depth = 0;
    while (!current_layer.empty() && (depth_limit < 0 || depth < depth_limit)) {
        std::vector<Cube4x4> next_layer;
        next_layer.reserve(current_layer.size() * moveset.size()); 

        #pragma omp parallel
        {
            std::vector<std::pair<std::string, Cube4x4>> local_new;
            #pragma omp for schedule(dynamic)
            for (int i = 0; i < (int)current_layer.size(); i++) {
                Cube4x4 parent_cube = current_layer[i];
                int parent_depth = 0; 
                std::string parent_mask = phase.mask(parent_cube);
                auto it = visited.find(parent_mask);
                if (it == visited.end()) {
                    continue;
                }
                parent_depth = it->second;

                if (depth_limit < 0 || parent_depth < depth_limit) {
                    for (auto &mv : moveset) {
                        Cube4x4 child_cube = parent_cube;
                        child_cube.move(mv);
                        std::string child_mask = phase.mask(child_cube);
                        local_new.emplace_back(child_mask, child_cube);
                    }
                }
            }

            #pragma omp critical
            {
                for (auto &pair : local_new) {
                    const std::string &child_mask = pair.first;
                    Cube4x4 &child_cube = pair.second;
                    auto it2 = visited.find(child_mask);
                    if (it2 == visited.end()) {
                        visited[child_mask] = depth + 1;
                        bloom.add(child_mask);
                        next_layer.push_back(std::move(child_cube));
                    }
                }
            }
        } 
        depth++;
        current_layer = std::move(next_layer);
        std::cout << "Reached depth " << depth << " | " << visited.size() << " states so far" << std::endl;
    }
    std::cout << "BFS done. Final depth reached: " << depth << " | Total states found: " << visited.size() << std::endl;
    std::string bloom_filename = phase.table_filename + ".bloom";
    bloom.save(bloom_filename);
    std::cout << "Saved Bloom filter to " << bloom_filename << std::endl;
    return visited;
}



int main() {         
    

    // phase 1,5,6,7,8 are working perfectly
    // phase 2 mask needs tweaking to make smaller state space
    // phase 3,4 masks take a long time to compute

    // phase 2 mask does not account for parities (but it might)
    // phase 4 mask does account for parities (but it might)

    std::cout << "Generating " << phase1.name << " tables...\n";
    Cube4x4 cube1;
    auto table1 = generate_table_layered_omp(phase1, {cube1}, phase1.moves, phase1.table_depth_limit); // 735,471 at 8
    std::string mph_file1 = phase1.table_filename + ".mph";
    std::string depth_file1 = phase1.table_filename + ".depths";
    build_and_save_mph(table1, mph_file1, depth_file1);

    std::cout << "Generating " << phase2.name << " tables...\n";
    Cube4x4 centre_cube;
    std::vector<Cube4x4> centre_table = generate_cube_table(phase2, {centre_cube}, {R2, L2, F, B, U, D, r2, l2, f2, b2, u2, d2}, -1); // 12  at 3
    auto table2 = generate_table_layered_omp(phase2, centre_table, phase2.moves, phase2.table_depth_limit); // 3,695,452 at 5 (limited)
    std::string mph_file2 = phase2.table_filename + ".mph";
    std::string depth_file2 = phase2.table_filename + ".depths";
    build_and_save_mph(table2, mph_file2, depth_file2);

    std::cout << "Generating " << phase3.name << " tables...\n";
    Cube4x4 centre_column_cube;
    std::vector<Cube4x4> centre_column_table = generate_cube_table(phase3, {centre_column_cube}, {R2, L2, F2, B2, U, U_PRIME, U2, D, D_PRIME, D2, r2, l2, f2, b2}, 4); // 36 at 4
    auto table3 = generate_table_layered_omp(phase3, centre_column_table, phase3.moves, phase3.table_depth_limit); // 215,028 at 13
    std::string mph_file3 = phase3.table_filename + ".mph";
    std::string depth_file3 = phase3.table_filename + ".depths";
    build_and_save_mph(table3, mph_file3, depth_file3);

    std::cout << "Generating " << phase4.name << " tables...\n";
    Cube4x4 cube4;
    auto table4 = generate_table_layered_omp(phase4, {cube4}, phase4.moves, phase4.table_depth_limit); // 12,089,495 at 9
    std::string mph_file4 = phase4.table_filename + ".mph";
    std::string depth_file4 = phase4.table_filename + ".depths";
    build_and_save_mph(table4, mph_file4, depth_file4);

    Cube4x4 cube5;
    std::cout << "Generating " << phase5.name << " tables...\n";
    auto table5 = generate_table_layered_omp(phase5, {cube5}, phase5.moves, phase5.table_depth_limit); // 2,048 at 8
    std::string mph_file5 = phase5.table_filename + ".mph";
    std::string depth_file5 = phase5.table_filename + ".depths";
    build_and_save_mph(table5, mph_file5, depth_file5);

    Cube4x4 cube6;
    std::cout << "Generating " << phase6.name << " tables...\n";
    auto table6 = generate_table_layered_omp(phase6, {cube6}, phase6.moves, phase6.table_depth_limit); // 1,082,565 at 11
    std::string mph_file6 = phase6.table_filename + ".mph";
    std::string depth_file6 = phase6.table_filename + ".depths";
    build_and_save_mph(table6, mph_file6, depth_file6);
    
    std::cout << "Generating " << phase7.name << " tables...\n";
    Cube4x4 corner_cube;
    std::vector<Cube4x4> corner_table = generate_cube_table(phase7, {corner_cube}, {R2, L2, F2, B2, U2, D2}, 5); // 96 at 4
    auto table7 = generate_table_layered_omp(phase7, corner_table, phase7.moves, phase7.table_depth_limit);  // 2,822,398 at 14
    std::string mph_file7 = phase7.table_filename + ".mph";
    std::string depth_file7 = phase7.table_filename + ".depths";
    build_and_save_mph(table7, mph_file7, depth_file7);

    std::cout << "Generating " << phase8.name << " tables...\n";
    Cube4x4 cube8;
    auto table8 = generate_table_layered_omp(phase8, {cube8}, phase8.moves, phase8.table_depth_limit); // 663,552 at 15
    std::string mph_file8 = phase8.table_filename + ".mph";
    std::string depth_file8 = phase8.table_filename + ".depths";
    build_and_save_mph(table8, mph_file8, depth_file8);

    return 0;
}