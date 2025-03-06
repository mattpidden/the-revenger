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
    std::array<Phase, 1> phases = {phase5};

    for (const Phase& phase : phases) {
        Cube4x4 phase_cube;
        std::vector<std::string> solved_states = {phase.mask(phase_cube)};
        
        auto table = generate_table(solved_states, phase.moves, phase.table_depth_limit);
        save_table_binary(table, phase.table_filename);
    }

    return 0;
}