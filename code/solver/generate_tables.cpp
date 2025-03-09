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

class BloomFilter {
public:
    BloomFilter(size_t size_in_bits, int num_hashes) : bitset(size_in_bits, false), size(size_in_bits), num_hashes(num_hashes) {}

    void add(const std::string &s) {
        for (int i = 0; i < num_hashes; i++) {
            size_t hash_val = hash(s, i);
            bitset[hash_val % size] = true;
        }
    }

    bool contains(const std::string &s) const {
        for (int i = 0; i < num_hashes; i++) {
            size_t hash_val = hash(s, i);
            if (!bitset[hash_val % size])
                return false;
        }
        return true;
    }

private:
    std::vector<bool> bitset;
    size_t size;
    int num_hashes;

    size_t hash(const std::string &s, int seed) const {
        std::hash<std::string> hasher;
        size_t h1 = hasher(s);
        return h1 ^ (static_cast<size_t>(seed) * 0x9e3779b97f4a7c15ULL);
    }
};

void save_table_binary(const std::unordered_map<std::string, int>& table, const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    size_t table_size = table.size();
    file.write(reinterpret_cast<const char*>(&table_size), sizeof(table_size));

    for (const auto& [state, depth] : table) {
        file.write(state.c_str(), 96); 
        file.write(reinterpret_cast<const char*>(&depth), sizeof(depth));
    }

    file.close();
}

std::unordered_map<std::string, int> load_table_binary(const std::string& filename) {
    std::unordered_map<std::string, int> table;
    std::ifstream file(filename, std::ios::binary);
    size_t table_size;
    file.read(reinterpret_cast<char*>(&table_size), sizeof(table_size));

    for (size_t i = 0; i < table_size; i++) {
        char state[97] = {0};
        int depth;
        file.read(state, 96);
        file.read(reinterpret_cast<char*>(&depth), sizeof(depth));
        table[state] = depth;
    }
    file.close();
    return table;
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
            std::cout << "Reached depth " << max_depth << " | States so far: " << table.size() << "\n";
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
    
    std::cout << "Final depth reached: " << max_depth << "\n";
    std::cout << "Total states found: " << table.size() << "\n";
    std::cout << "Total cubes found: " << cube_table.size() << "\n";
    
    return cube_table;
}

void generate_table(const Phase &phase, const std::vector<Cube4x4> solved_cubes, const std::vector<Move>& moveset, int depth_limit = -1) {
    BloomFilter bf(100000000, 7);
    std::unordered_map<std::string, int> table;
    std::vector<Cube4x4> current_layer;
    int depth = 0;
    
    for (Cube4x4 cube : solved_cubes) {
        std::string state = phase.mask(cube);
        bf.add(state);
        table[state] = 0;
        current_layer.push_back(cube);
    }
    save_table_binary(table, phase.table_filename);
    std::cout << "Reached depth " << depth << " | States so far: " << table.size() << "\n";
    
    while (!current_layer.empty() && (depth_limit < 0 || depth < depth_limit)) {
        std::vector<Cube4x4> next_layer;

        for (const Cube4x4 &current_cube : current_layer) {
            for (const auto &move : moveset) {
                Cube4x4 next_cube = current_cube;
                next_cube.move(move);
                std::string next_state = phase.mask(next_cube);
                if (!bf.contains(next_state)) {
                    bf.add(next_state);
                    table[next_state] = depth + 1;
                    next_layer.push_back(next_cube);
                }
            }
        }

        depth++;
        std::unordered_map<std::string, int> total_table = load_table_binary(phase.table_filename);
        for (const auto& [state, depth] : table) {
            total_table[state] = depth;
        }
        table.clear();
        save_table_binary(total_table, phase.table_filename);
        std::cout << "Reached depth " << depth << " | States so far: " << total_table.size() << "\n";

        // TODO empty current_layer, replace it with next_layer and then clear next_layer
        current_layer = std::move(next_layer);
    }
    
    std::cout << "Found all states. \n\n";
    
    return;
}





int main() {         
    Cube4x4 cube;

    // std::cout << "Generating " << phase1.name << " tables...\n";
    // generate_table(phase1, {cube}, phase1.moves, phase1.table_depth_limit); // 735,471 at 9

    std::cout << "Generating " << phase2.name << " tables...\n";
    Cube4x4 centre_cube;
    std::vector<Cube4x4> centre_table = generate_cube_table(phase2, {centre_cube}, {R2, L2, F, B, U, D, r2, l2, f2, b2, u2, d2}, 3); // 88  at 3 (should be 12)
    generate_table(phase2, centre_table, phase2.moves, phase2.table_depth_limit); // 1,914,822 at 4

    // std::cout << "Generating " << phase3.name << " tables...\n";
    // Cube4x4 centre_column_cube;
    // std::vector<Cube4x4> centre_column_table = generate_cube_table(phase3, {centre_column_cube}, {R2, L2, F2, B2, U, U_PRIME, U2, D, D_PRIME, D2, r2, l2, f2, b2}, 4); //36, 4
    // generate_table(phase3, centre_column_table, phase3.moves, phase3.table_depth_limit); // 215,028 at 14

    std::cout << "Generating " << phase4.name << " tables...\n";
    generate_table(phase4, {cube}, phase4.moves, phase4.table_depth_limit); // 

    // std::cout << "Generating " << phase5.name << " tables...\n";
    // generate_table(phase5, {cube}, phase5.moves, phase5.table_depth_limit); // 2,048 at 8

    // std::cout << "Generating " << phase6.name << " tables...\n";
    // generate_table(phase6, {cube}, phase6.moves, phase6.table_depth_limit); // 1,082,565 at 11
    
    // std::cout << "Generating " << phase7.name << " tables...\n";
    // Cube4x4 corner_cube;
    // std::vector<Cube4x4> corner_table = generate_cube_table(phase7, {corner_cube}, {R2, L2, F2, B2, U2, D2}, 5); // 96 at 4
    // generate_table(phase7, corner_table, phase7.moves, phase7.table_depth_limit); // 2,822,398 at 14

    // std::cout << "Generating " << phase8.name << " tables...\n";
    // generate_table(phase8, {cube}, phase8.moves, phase8.table_depth_limit); // 663,552 at 16

    return 0;
}