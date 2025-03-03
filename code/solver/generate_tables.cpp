#include <vector>
#include <limits>
#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>
#include <cmath>
#include <fstream>
#include <unordered_map>
#include "cube.h"

std::string apply_move_to_state(const std::string& state, Move move) {
  Cube4x4 cube;
  cube.import_state(state);
  cube.move(move);
  return cube.export_state();
}

std::unordered_map<std::string, int> generate_table(const std::vector<std::string>& solved_states, int depth, const std::vector<Move>& moveset) {
    std::unordered_map<std::string, int> table;
    std::vector<std::string> previous_frontier(solved_states.begin(), solved_states.end());

    for (const auto& state : solved_states) {
        table[state] = 0;
    }

    for (int i = 1; i <= depth; i++) {
        std::vector<std::string> frontier;
        for (const auto& state : previous_frontier) {
            for (const auto& move : moveset) {
                std::string new_state = apply_move_to_state(state, move);
                if (table.find(new_state) == table.end()) {  
                    table[new_state] = i;
                    frontier.push_back(new_state);
                }
            }
        }
        previous_frontier = std::move(frontier); 
    }

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
    // Cube4x4 cube;
    // std::vector<std::string> phase8_solved_states = {cube.export_state()};
    // std::vector<Move> phase8_moves = {R2, L2, F2, B2, U2, D2};
    // auto phase8table = generate_table(phase8_solved_states, 14, phase8_moves);
    // save_table_binary(phase8table, "phase8table.bin");

    Cube4x4 cube;
    std::vector<int> phase5_mask = {1,2,4,7,8,11,13,14,36,39,40,43,68,71,72,75,81,82,84,87,88,91,93,94};
    std::vector<std::string> phase5_solved_states = {cube.apply_mask_colours(phase5_mask)};
    std::vector<Move> phase5_moves = {R, L, F, B, U, D};
    auto phase5table = generate_table(phase5_solved_states, 10, phase5_moves);
    save_table_binary(phase5table, "phase5table.bin");

    return 0;
}