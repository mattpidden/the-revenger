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

#include "cube.h"

std::string apply_move_to_state(const std::string& state, Move move) {
  Cube4x4 cube;
  cube.import_state(state); // this function does not exist
  cube.move(move);
  return cube.export_state();
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
            std::cout << "Reached depth " << max_depth << " | States so far: " << table.size() << "\n";
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
    
    std::cout << "Final depth reached: " << max_depth << "\n";
    std::cout << "Total states found: " << table.size() << "\n";
    
    return table;
}


void save_table_binary(const std::unordered_map<std::string, int>& table, const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    size_t table_size = table.size();
    file.write(reinterpret_cast<const char*>(&table_size), sizeof(table_size));

    for (const auto& [state, depth] : table) {
        file.write(state.c_str(), 96); 
        file.write(reinterpret_cast<const char*>(&depth), sizeof(depth));
    }

    file.close();
    std::cout << "Saved table to " << filename << " (binary format)\n\n";
}

int main() {         
    Cube4x4 cube;

    // std::cout << "Generating " << phase1.name << " tables...\n";
    // auto table1 = generate_table(phase1, {cube}, phase1.moves, phase1.table_depth_limit);
    // save_table_binary(table1, phase1.table_filename);

    // std::cout << "Generating " << phase2.name << " tables...\n";
    // Cube4x4 centre_cube;
    // std::vector<Cube4x4> centre_table = generate_cube_table(phase2, {centre_cube}, {R2, L2, F, B, U, D, r2, l2, f2, b2, u2, d2}, 3);
    // auto table2 = generate_table(phase2, centre_table, phase2.moves, phase2.table_depth_limit);
    // save_table_binary(table2, phase2.table_filename);
    
    std::cout << "Generating " << phase3.name << " tables...\n";
    Cube4x4 centre_column_cube;
    std::vector<Cube4x4> centre_column_table = generate_cube_table(phase3, {centre_column_cube}, {R2, L2, F2, B2, U, U_PRIME, U2, D, D_PRIME, D2, r2, l2, f2, b2}, 10);
    auto table3 = generate_table(phase3, centre_column_table, phase3.moves, phase3.table_depth_limit);
    save_table_binary(table3, phase3.table_filename);

    std::cout << "Generating " << phase4.name << " tables...\n";
    auto table4 = generate_table(phase4, {cube}, phase4.moves, phase4.table_depth_limit);
    save_table_binary(table4, phase4.table_filename);

    // std::cout << "Generating " << phase5.name << " tables...\n";
    // auto table5 = generate_table(phase5, {cube}, phase5.moves, phase5.table_depth_limit);
    // save_table_binary(table5, phase5.table_filename);

    // std::cout << "Generating " << phase6.name << " tables...\n";
    // auto table6 = generate_table(phase6, {cube}, phase6.moves, phase6.table_depth_limit);
    // save_table_binary(table6, phase6.table_filename);
    
    // std::cout << "Generating " << phase7.name << " tables...\n";
    // Cube4x4 corner_cube;
    // std::vector<Cube4x4> corner_table = generate_cube_table(phase7, {corner_cube}, {R2, L2, F2, B2, U2, D2}, 5);
    // auto table7 = generate_table(phase7, corner_table, phase7.moves, phase7.table_depth_limit);
    // save_table_binary(table7, phase7.table_filename);

    // std::cout << "Generating " << phase8.name << " tables...\n";
    // auto table8 = generate_table(phase8, {cube}, phase8.moves, phase8.table_depth_limit);
    // save_table_binary(table8, phase8.table_filename);

    return 0;
}