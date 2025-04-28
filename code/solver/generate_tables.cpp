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
#include <sys/resource.h>
#include <string_view>
#include "parallel_hashmap/phmap.h"

using namespace boomphf;
using phmap::flat_hash_map;

void save_depths(const std::vector<char> &depths, const std::string &filename) {
    std::ofstream out(filename, std::ios::binary);
    uint64_t sz = depths.size();
    out.write(reinterpret_cast<const char*>(&sz), sizeof(sz));
    out.write(reinterpret_cast<const char*>(depths.data()), depths.size());
}

void build_and_save_mph(const flat_hash_map<std::string,char> &table, const std::string &filename, double gamma) {
    std::string mph_filename = filename + ".mph";
    std::string depth_filename = filename + ".depths";
    std::vector<std::string> keys;
    keys.reserve(table.size());
    for (auto &kv : table) {
        keys.push_back(kv.first);
    }
    auto mph = new boomphf::mphf<std::string, MyStringHash>(
        keys.size(),   // # of keys
        keys,          // pass the vector
        1,             // # of threads
        gamma,         // gamma
        false,         // writeEach
        false,         // progress
        0.05f          // alpha ratio
    );
    std::vector<char> depths(keys.size(), -1);
    for (auto &kv : table) {
        uint64_t idx = mph->lookup(kv.first);
        depths[idx] = kv.second;
    }
    save_depths(depths, depth_filename);
    std::ofstream ofs(mph_filename, std::ios::binary);
    mph->save(ofs);
    delete mph;
    std::cout << "BBHash MPH built & saved to " << mph_filename << " and " << depth_filename << "\n" << std::endl;
}

void print_memory_usage() {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    std::cout << "Memory usage: " << (usage.ru_maxrss / (1024.0 * 1024.0)) << " GB" << std::endl;
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
            std::cout << "Reached depth " << static_cast<int>(max_depth) << " | States so far: " << table.size() << std::endl;
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
    
    std::cout << "Final depth reached: " << static_cast<int>(max_depth) << std::endl;
    std::cout << "Total states found: " << table.size() << std::endl;
    std::cout << "Total cubes found: " << cube_table.size() << std::endl;
    
    return cube_table;
}

flat_hash_map<std::string, char> generate_table(const Phase &phase, const std::vector<Cube4x4> solved_cubes, const std::vector<Move>& moveset, char depth_limit = -1) {
    flat_hash_map<std::string, char> table;
    std::queue<Cube4x4> frontier;
    
    for (Cube4x4 cube : solved_cubes) {
        std::string state = phase.mask(cube);
        table[state] = 0;
        frontier.push(cube);
    }
    
    char max_depth = 0;
    while (!frontier.empty()) {
        Cube4x4 current_cube = frontier.front();
        frontier.pop();
        std::string current_state = phase.mask(current_cube);
        char depth = table[current_state];
        
        if (depth > max_depth) {
            max_depth = depth;
            std::cout << "Reached depth " << static_cast<int>(max_depth) << " | States so far: " << table.size() << std::endl;
            print_memory_usage();
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
    
    std::cout << "Final depth reached: " << static_cast<int>(max_depth) << std::endl;
    std::cout << "Total states found: " << table.size() << std::endl;
    
    return table;
}




int main() {         
    std::cout << "Generating " << phase1.name << " tables...\n";
    Cube4x4 cube1;
    auto table1 = generate_table(phase1, {cube1}, phase1.moves, phase1.table_depth_limit);
    build_and_save_mph(table1, phase1.table_filename, phase1.gamma);

    std::cout << "Generating " << phase2a.name << " tables...\n";
    Cube4x4 cube2a;
    auto table2a = generate_table(phase2a, {cube2a}, phase2a.moves, phase2a.table_depth_limit);
    build_and_save_mph(table2a, phase2a.table_filename, phase2a.gamma);

    std::cout << "Generating " << phase2b.name << " tables...\n";
    Cube4x4 centre_cube;
    std::vector<Cube4x4> centre_table = generate_cube_table(phase2b, {centre_cube}, {R2, L2, F, B, U, D, r2, l2, f2, b2, u2, d2}, -1); // TODO do all of these have correct parity?
    auto table2b = generate_table(phase2b, centre_table, phase2b.moves, phase2b.table_depth_limit); 
    build_and_save_mph(table2b, phase2b.table_filename, phase3.gamma);

    std::cout << "Generating " << phase3.name << " tables...\n";
    Cube4x4 centre_column_cube;
    std::vector<Cube4x4> centre_column_table = generate_cube_table(phase3, {centre_column_cube}, {R2, L2, F2, B2, U, U_PRIME, U2, D, D_PRIME, D2, r2, l2, f2, b2}, -1); 
    auto table3 = generate_table(phase3, centre_column_table, phase3.moves, phase3.table_depth_limit); 
    build_and_save_mph(table3, phase3.table_filename, phase3.gamma);
    
    std::cout << "Generating " << phase4.name << " tables...\n";
    Cube4x4 cube4;
    auto table4 = generate_table(phase4, {cube4}, phase4.moves, phase4.table_depth_limit); 
    build_and_save_mph(table4, phase4.table_filename, phase4.gamma);

    Cube4x4 cube6;
    std::cout << "Generating " << phase6.name << " tables...\n";
    auto table6 = generate_table(phase6, {cube6}, phase6.moves, phase6.table_depth_limit); 
    build_and_save_mph(table6, phase6.table_filename, phase6.gamma);
    
    std::cout << "Generating " << phase7.name << " tables...\n";
    Cube4x4 corner_cube;
    std::vector<Cube4x4> corner_table = generate_cube_table(phase7, {corner_cube}, {R2, L2, F2, B2, U2, D2}, -1);
    auto table7 = generate_table(phase7, corner_table, phase7.moves, phase7.table_depth_limit); 
    build_and_save_mph(table7, phase7.table_filename, phase7.gamma);

    std::cout << "Generating " << phase8.name << " tables...\n";
    Cube4x4 cube8;
    auto table8 = generate_table(phase8, {cube8}, phase8.moves, phase8.table_depth_limit); 
    build_and_save_mph(table8, phase8.table_filename, phase8.gamma);

    return 0;
}