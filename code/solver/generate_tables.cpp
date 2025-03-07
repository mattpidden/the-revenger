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
  cube.import_state(state);
  cube.move(move);
  return cube.export_state();
}

std::unordered_map<std::string, int> generate_table(const std::vector<std::string>& solved_states, const std::vector<Move>& moveset, int depth_limit = -1) {
    std::unordered_map<std::string, int> table;
    std::vector<std::string> previous_frontier(solved_states.begin(), solved_states.end());

    std::queue<std::string> frontier;
    for (const auto& state : solved_states) {
        table[state] = 0;
        frontier.push(state);
    }

    int max_depth = 0;
    while (!frontier.empty()) {
        std::string current = frontier.front();
        frontier.pop();
        int depth = table[current];

        if (depth > max_depth) {
            max_depth = depth;
            std::cout << "Reached depth " << max_depth << " | States so far: " << table.size() << "\n";
        }

        if (depth_limit < 0 || depth < depth_limit) {
            for (const auto& move : moveset) {
                std::string next = apply_move_to_state(current, move);
                if (table.find(next) == table.end()) {
                    table[next] = depth + 1;
                    frontier.push(next);
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
    std::cout << "Saved table to " << filename << " (binary format)\n";
}

int main() { 
    std::vector<Phase> phases = {phase3};

    // This generated 96 starting stats for phase 7 (corner paritys)
    // Cube4x4 corner_cube;
    // std::function<std::string(Cube4x4&)> phase7_mask = [](Cube4x4& cube) -> std::string {
    //     std::vector<int> mask = {0,3,12,15,16,17,18,19,20,23,24,27,28,29,30,31,32,33,34,35,36,39,40,43,44,45,46,47,48,49,50,51,52,55,56,59,60,61,62,63,64,65,66,67,68,71,72,75,76,77,78,79,80,83,92,95};
    //     std::vector<int> colour_mask1 = {17,18,20,23,24,27,29,30,49,50,52,55,56,59,61,62};
    //     std::vector<int> colour_mask2 = {33,34,36,39,40,43,45,46,65,66,68,71,72,75,77,78};
    //     cube.apply_mask(mask);
    //     cube.apply_colour_mask(-3, colour_mask1);
    //     cube.apply_colour_mask(-4, colour_mask2);
    //     return cube.export_state();
    // };
    // std::unordered_map<std::string, int> corner_table = generate_table({phase7_mask(corner_cube)}, {R2, L2, F2, B2, U2, D2}, 10);
    // for (const auto& [state, depth] : corner_table) {
    //     std::cout << '"' << state << "\",\n";
    // }

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

    for (const Phase& phase : phases) {        
        auto table = generate_table(phase.solved_states, phase.moves, phase.table_depth_limit);
        save_table_binary(table, phase.table_filename);
    }

    return 0;
}