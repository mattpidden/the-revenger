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
    // This generated 12 starting stats for phase 2 (centre states)
    // Cube4x4 centre_cube;
    // std::function<std::string(Cube4x4&)> phase2_mask = [](Cube4x4& cube) -> std::string {
    //     std::vector<int> mask = {5,6,9,10,85,86,89,90,37,38,41,42,69,70,73,74,21,22,25,26,53,54,57,58};
    //     std::vector<int> colour_mask1 = {5,6,9,10,85,86,89,90};
    //     std::vector<int> colour_mask2 = {37,38,41,42,69,70,73,74};
    //     cube.apply_mask(mask);
    //     cube.apply_colour_mask(-3, colour_mask1);
    //     cube.apply_colour_mask(-4, colour_mask2);
    //     return cube.export_state();
    // };
    // std::unordered_map<std::string, int> centre_table = generate_table({phase2_mask(centre_cube)}, {R2, L2, F, B, U, D, r2, l2, f2, b2, u2, d2}, 10);
    // for (const auto& [state, depth] : centre_table) {
    //     std::cout << '"' << state << "\",\n";
    // }

    // This generated 36 starting stats for phase 3 (centre columns with paired edges)
//     Cube4x4 centre_cube;
//     std::function<std::string(Cube4x4&)> phase3_mask = [](Cube4x4& cube) -> std::string {
//         std::vector<int> mask = {37,38,41,42,69,70,73,74,21,22,25,26,53,54,57,58};
//         cube.apply_mask(mask);
//         return cube.export_state();
//     };
//     std::unordered_map<std::string, int> centre_table = generate_table({phase3_mask(centre_cube)}, {R2, L2, F2, B2, U, U_PRIME, U2, D, D_PRIME, D2, r2, l2, f2, b2}, 10);
//     for (const auto& [fixed_state, depth] : centre_table) {
//         std::string state = fixed_state; 
//         std::vector<int> edge_facelets = {20,24,23,27,36,40,39,43,52,56,55,59,68,72,71,75};
//         for (const int facelet : edge_facelets) {
//             //state[facelet] = '3';
//         }
//         std::cout << '"' << state << "\",\n";
//     }
//     std::unordered_map<std::string, int> phase3_table = generate_table({"XXXXXXXXXXXXXXXXXXXXXRRXXRRXXXXXXXXXXGBXXGBXXXXXXXXXXOOXXOOXXXXXXXXXXBGXXBGXXXXXXXXXXXXXXXXXXXXX",
// "XXXXXXXXXXXXXXXXXXXXXROXXROXXXXXXXXXXGBXXGBXXXXXXXXXXORXXORXXXXXXXXXXBGXXBGXXXXXXXXXXXXXXXXXXXXX",
// "XXXXXXXXXXXXXXXXXXXXXROXXROXXXXXXXXXXBBXXBBXXXXXXXXXXORXXORXXXXXXXXXXGGXXGGXXXXXXXXXXXXXXXXXXXXX",
// "XXXXXXXXXXXXXXXXXXXXXORXXORXXXXXXXXXXGBXXGBXXXXXXXXXXROXXROXXXXXXXXXXBGXXBGXXXXXXXXXXXXXXXXXXXXX",
// "XXXXXXXXXXXXXXXXXXXXXORXXORXXXXXXXXXXGGXXGGXXXXXXXXXXROXXROXXXXXXXXXXBBXXBBXXXXXXXXXXXXXXXXXXXXX",
// "XXXXXXXXXXXXXXXXXXXXXRRXXRRXXXXXXXXXXBBXXBBXXXXXXXXXXOOXXOOXXXXXXXXXXGGXXGGXXXXXXXXXXXXXXXXXXXXX",
// "XXXXXXXXXXXXXXXXXXXXXROXXROXXXXXXXXXXGGXXGGXXXXXXXXXXROXXROXXXXXXXXXXBBXXBBXXXXXXXXXXXXXXXXXXXXX",
// "XXXXXXXXXXXXXXXXXXXXXRRXXRRXXXXXXXXXXBGXXBGXXXXXXXXXXOOXXOOXXXXXXXXXXBGXXBGXXXXXXXXXXXXXXXXXXXXX",
// "XXXXXXXXXXXXXXXXXXXXXROXXROXXXXXXXXXXGBXXGBXXXXXXXXXXROXXROXXXXXXXXXXGBXXGBXXXXXXXXXXXXXXXXXXXXX",
// "XXXXXXXXXXXXXXXXXXXXXORXXORXXXXXXXXXXBGXXBGXXXXXXXXXXORXXORXXXXXXXXXXBGXXBGXXXXXXXXXXXXXXXXXXXXX",
// "XXXXXXXXXXXXXXXXXXXXXORXXORXXXXXXXXXXGBXXGBXXXXXXXXXXORXXORXXXXXXXXXXGBXXGBXXXXXXXXXXXXXXXXXXXXX",
// "XXXXXXXXXXXXXXXXXXXXXOOXXOOXXXXXXXXXXGBXXGBXXXXXXXXXXRRXXRRXXXXXXXXXXBGXXBGXXXXXXXXXXXXXXXXXXXXX",
// "XXXXXXXXXXXXXXXXXXXXXOOXXOOXXXXXXXXXXBBXXBBXXXXXXXXXXRRXXRRXXXXXXXXXXGGXXGGXXXXXXXXXXXXXXXXXXXXX",
// "XXXXXXXXXXXXXXXXXXXXXOOXXOOXXXXXXXXXXBGXXBGXXXXXXXXXXRRXXRRXXXXXXXXXXGBXXGBXXXXXXXXXXXXXXXXXXXXX",
// "XXXXXXXXXXXXXXXXXXXXXORXXORXXXXXXXXXXBBXXBBXXXXXXXXXXROXXROXXXXXXXXXXGGXXGGXXXXXXXXXXXXXXXXXXXXX",
// "XXXXXXXXXXXXXXXXXXXXXORXXORXXXXXXXXXXBBXXBBXXXXXXXXXXORXXORXXXXXXXXXXGGXXGGXXXXXXXXXXXXXXXXXXXXX",
// "XXXXXXXXXXXXXXXXXXXXXROXXROXXXXXXXXXXGBXXGBXXXXXXXXXXORXXORXXXXXXXXXXGBXXGBXXXXXXXXXXXXXXXXXXXXX",
// "XXXXXXXXXXXXXXXXXXXXXOOXXOOXXXXXXXXXXBGXXBGXXXXXXXXXXRRXXRRXXXXXXXXXXBGXXBGXXXXXXXXXXXXXXXXXXXXX",
// "XXXXXXXXXXXXXXXXXXXXXROXXROXXXXXXXXXXBGXXBGXXXXXXXXXXROXXROXXXXXXXXXXBGXXBGXXXXXXXXXXXXXXXXXXXXX",
// "XXXXXXXXXXXXXXXXXXXXXOOXXOOXXXXXXXXXXGGXXGGXXXXXXXXXXRRXXRRXXXXXXXXXXBBXXBBXXXXXXXXXXXXXXXXXXXXX",
// "XXXXXXXXXXXXXXXXXXXXXORXXORXXXXXXXXXXGGXXGGXXXXXXXXXXORXXORXXXXXXXXXXBBXXBBXXXXXXXXXXXXXXXXXXXXX",
// "XXXXXXXXXXXXXXXXXXXXXROXXROXXXXXXXXXXGGXXGGXXXXXXXXXXORXXORXXXXXXXXXXBBXXBBXXXXXXXXXXXXXXXXXXXXX",
// "XXXXXXXXXXXXXXXXXXXXXORXXORXXXXXXXXXXBGXXBGXXXXXXXXXXORXXORXXXXXXXXXXGBXXGBXXXXXXXXXXXXXXXXXXXXX",
// "XXXXXXXXXXXXXXXXXXXXXROXXROXXXXXXXXXXBGXXBGXXXXXXXXXXROXXROXXXXXXXXXXGBXXGBXXXXXXXXXXXXXXXXXXXXX",
// "XXXXXXXXXXXXXXXXXXXXXRRXXRRXXXXXXXXXXBGXXBGXXXXXXXXXXOOXXOOXXXXXXXXXXGBXXGBXXXXXXXXXXXXXXXXXXXXX",
// "XXXXXXXXXXXXXXXXXXXXXORXXORXXXXXXXXXXGBXXGBXXXXXXXXXXORXXORXXXXXXXXXXBGXXBGXXXXXXXXXXXXXXXXXXXXX",
// "XXXXXXXXXXXXXXXXXXXXXROXXROXXXXXXXXXXBGXXBGXXXXXXXXXXORXXORXXXXXXXXXXBGXXBGXXXXXXXXXXXXXXXXXXXXX",
// "XXXXXXXXXXXXXXXXXXXXXROXXROXXXXXXXXXXGBXXGBXXXXXXXXXXROXXROXXXXXXXXXXBGXXBGXXXXXXXXXXXXXXXXXXXXX",
// "XXXXXXXXXXXXXXXXXXXXXOOXXOOXXXXXXXXXXGBXXGBXXXXXXXXXXRRXXRRXXXXXXXXXXGBXXGBXXXXXXXXXXXXXXXXXXXXX",
// "XXXXXXXXXXXXXXXXXXXXXRRXXRRXXXXXXXXXXGBXXGBXXXXXXXXXXOOXXOOXXXXXXXXXXGBXXGBXXXXXXXXXXXXXXXXXXXXX",
// "XXXXXXXXXXXXXXXXXXXXXRRXXRRXXXXXXXXXXGGXXGGXXXXXXXXXXOOXXOOXXXXXXXXXXBBXXBBXXXXXXXXXXXXXXXXXXXXX",
// "XXXXXXXXXXXXXXXXXXXXXORXXORXXXXXXXXXXBGXXBGXXXXXXXXXXROXXROXXXXXXXXXXBGXXBGXXXXXXXXXXXXXXXXXXXXX",
// "XXXXXXXXXXXXXXXXXXXXXORXXORXXXXXXXXXXGBXXGBXXXXXXXXXXROXXROXXXXXXXXXXGBXXGBXXXXXXXXXXXXXXXXXXXXX",
// "XXXXXXXXXXXXXXXXXXXXXORXXORXXXXXXXXXXBGXXBGXXXXXXXXXXROXXROXXXXXXXXXXGBXXGBXXXXXXXXXXXXXXXXXXXXX",
// "XXXXXXXXXXXXXXXXXXXXXROXXROXXXXXXXXXXBBXXBBXXXXXXXXXXROXXROXXXXXXXXXXGGXXGGXXXXXXXXXXXXXXXXXXXXX",
// "XXXXXXXXXXXXXXXXXXXXXROXXROXXXXXXXXXXBGXXBGXXXXXXXXXXORXXORXXXXXXXXXXGBXXGBXXXXXXXXXXXXXXXXXXXXX",}, {R2, L2, F, F_PRIME, F2, B, B_PRIME, B2, U, U_PRIME, U2, D, D_PRIME, D2, r2, l2, f2, b2, u2, d2}, 10);
//     save_table_binary(phase3_table, "phase3table.bin");
// for (const auto& [fixed_state, depth] : phase3_table) {
//         std::string state = fixed_state; 
//         std::vector<int> edge_facelets = {20,24,23,27,36,40,39,43,52,56,55,59,68,72,71,75};
//         for (const int facelet : edge_facelets) {
//             //state[facelet] = '3';
//         }
//         std::cout << '"' << state << "\",\n";
//     }



        //auto table = generate_table(phase.solved_states, phase.moves, phase.table_depth_limit);
        
    Cube4x4 cube;

    std::cout << "Generating " << phase1.name << " tables...\n";
    auto table1 = generate_table(phase1, {cube}, phase1.moves, phase1.table_depth_limit);
    save_table_binary(table1, phase1.table_filename);

    std::cout << "Generating " << phase5.name << " tables...\n";
    auto table5 = generate_table(phase5, {cube}, phase5.moves, phase5.table_depth_limit);
    save_table_binary(table5, phase5.table_filename);

    std::cout << "Generating " << phase6.name << " tables...\n";
    auto table6 = generate_table(phase6, {cube}, phase6.moves, phase6.table_depth_limit);
    save_table_binary(table6, phase6.table_filename);
    
    std::cout << "Generating " << phase7.name << " tables...\n";
    Cube4x4 corner_cube;
    std::vector<Cube4x4> corner_table = generate_cube_table(phase7, {corner_cube}, {R2, L2, F2, B2, U2, D2}, 10);
    auto table7 = generate_table(phase7, corner_table, phase7.moves, phase7.table_depth_limit);
    save_table_binary(table7, phase7.table_filename);

    std::cout << "Generating " << phase8.name << " tables...\n";
    auto table8 = generate_table(phase8, {cube}, phase8.moves, phase8.table_depth_limit);
    save_table_binary(table8, phase8.table_filename);

    return 0;
}