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
#include "cube.h"

std::string apply_move_to_state(const std::string& state, Move move) {
  Cube4x4 cube;
  cube.import_state(state);
  cube.move(move);
  return cube.export_state();
}

std::unordered_map<std::string, int> generate_table(const std::vector<std::string>& solved_states, const std::vector<Move>& moveset) {
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

        for (const auto& move : moveset) {
            std::string next = apply_move_to_state(current, move);
            if (table.find(next) == table.end()) {
                table[next] = depth + 1;
                frontier.push(next);
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

class Phase {
public:
    std::string name;
    std::vector<Move> moves;
    std::vector<int> mask;
    std::map<int, std::vector<int>> colour_mask;
    std::string table_filename;

    Phase(std::string name, const std::vector<Move>& moves, const std::vector<int>& mask, const std::map<int, std::vector<int>>& colour_mask, const std::string& table_filename) : name(name), moves(moves), mask(mask), colour_mask(colour_mask), table_filename(table_filename) {}
};

int main() {

    Phase phase1("Phase 1", {R, R_PRIME, R2, L, L_PRIME, L2, F, F_PRIME, F2, B, B_PRIME, B2, U, U_PRIME, U2, D, D_PRIME, D2, r, r_PRIME, r2, l, l_PRIME, l2, f, f_PRIME, f2, b, b_PRIME, b2, u, u_PRIME, u2, d, d_PRIME, d2}, {21,22,25,26,53,54,57,58}, {{-2, {21,22,25,26,53,54,57,58}}}, "phase1table.bin");
    //Phase phase2("Phase 2", {R, R_PRIME, R2, L, L_PRIME, L2, F, F_PRIME, F2, B, B_PRIME, B2, U, U_PRIME, U2, D, D_PRIME, D2, r, r_PRIME, r2, l, l_PRIME, l2, f2, b2, u2, d2}, {5,6,9,10,37,38,41,42,69,70,73,74,85,86,89,90}, {{-3, {5,6,9,10,85,86,89,90}}, {-4, {37,38,41,42,69,70,73,74}}}, "phase2table.bin");
    Phase phase2("Phase 2", {R, R_PRIME, R2, L, L_PRIME, L2, F, F_PRIME, F2, B, B_PRIME, B2, U, U_PRIME, U2, D, D_PRIME, D2, r, r_PRIME, r2, l, l_PRIME, l2, f2, b2, u2, d2}, {1,2,4,7,8,11,13,14,36,39,40,43,68,71,72,75,81,82,84,87,88,91,93,94,5,6,9,10,37,38,41,42,69,70,73,74,85,86,89,90}, {{-3, {5,6,9,10,85,86,89,90}}, {-4, {37,38,41,42,69,70,73,74}}, {-2, {1,2,4,7,8,11,13,14,36,39,40,43,68,71,72,75,81,82,84,87,88,91,93,94}}}, "phase2table.bin");
    Phase phase3("Phase 3", {R2, L2, F, F_PRIME, F2, B, B_PRIME, B2, U, U_PRIME, U2, D, D_PRIME, D2, r2, l2, f2, b2, u2, d2}, {1,2,4,7,8,11,13,14,36,39,40,43,68,71,72,75,81,82,84,87,88,91,93,94,5,6,9,10,37,38,41,42,69,70,73,74,85,86,89,90}, {{-3, {5,6,9,10,85,86,89,90}}, {-4, {37,38,41,42,69,70,73,74}}, {-2, {1,2,4,7,8,11,13,14,36,39,40,43,68,71,72,75,81,82,84,87,88,91,93,94}}}, "phase2table.bin");
    Phase phase4("Phase 4", {R2, L2, F2, B2, U, U_PRIME, U2, D, D_PRIME, D2, r2, l2, f2, b2}, {1,2,4,7,8,11,13,14,36,39,40,43,68,71,72,75,81,82,84,87,88,91,93,94,5,6,9,10,37,38,41,42,69,70,73,74,85,86,89,90}, {{-3, {5,6,9,10,85,86,89,90}}, {-4, {37,38,41,42,69,70,73,74}}, {-2, {1,2,4,7,8,11,13,14,36,39,40,43,68,71,72,75,81,82,84,87,88,91,93,94}}}, "phase2table.bin");
    
    Phase phase5("Phase 5", {R, R_PRIME, R2, L, L_PRIME, L2, F, F_PRIME, F2, B, B_PRIME, B2, U, U_PRIME, U2, D, D_PRIME, D2}, {1,2,4,7,8,11,13,14,36,39,40,43,68,71,72,75,81,82,84,87,88,91,93,94}, {{-2, {1,2,4,7,8,11,13,14,36,39,40,43,68,71,72,75,81,82,84,87,88,91,93,94}}}, "phase5table.bin");
    Phase phase6("Phase 6", {R, R_PRIME, R2, L, L_PRIME, L2, F2, B2, U, U_PRIME, U2, D, D_PRIME, D2}, {0,1,2,3,4,7,8,11,12,13,14,15,36,39,40,43,68,71,72,75,80,81,82,83,84,87,88,91,92,93,94,95}, {{-3, {0,1,2,3,4,7,8,11,12,13,14,15,80,81,82,83,84,87,88,91,92,93,94,95}}, {-4, {36,39,40,43,68,71,72,75}}}, "phase6table.bin");
    Phase phase7_corners("Phase 7 (corners)", {R2, L2, F2, B2, U2, D2}, {0,3,12,15,16,17,18,19,20,23,24,27,28,29,30,31,32,33,34,35,36,39,40,43,44,45,46,47,48,49,50,51,52,55,56,59,60,61,62,63,64,65,66,67,68,71,72,75,76,77,78,79,80,83,92,95}, {{-3, {17,18,20,23,24,27,29,30,49,50,52,55,56,59,61,62}}, {-4, {33,34,36,39,40,43,45,46,65,66,68,71,72,75,77,78}}}, "phase7cornertable.bin");
    Phase phase7("Phase 7", {R2, L2, F2, B2, U, U_PRIME, U2, D, D_PRIME, D2}, {0,3,12,15,16,17,18,19,20,23,24,27,28,29,30,31,32,33,34,35,36,39,40,43,44,45,46,47,48,49,50,51,52,55,56,59,60,61,62,63,64,65,66,67,68,71,72,75,76,77,78,79,80,83,92,95}, {{-3, {17,18,20,23,24,27,29,30,49,50,52,55,56,59,61,62}}, {-4, {33,34,36,39,40,43,45,46,65,66,68,71,72,75,77,78}}}, "phase7table.bin");
    Phase phase8("Phase 8", {R2, L2, F2, B2, U2, D2}, {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95}, {}, "phase8table.bin");

    std::array<Phase, 1> phases = {phase2};

    for (const Phase& phase : phases) {
        Cube4x4 phase_cube;
        phase_cube.print();
        phase_cube.apply_mask(phase.mask);
        phase_cube.print();
        for (const auto& [value, mask] : phase.colour_mask) {
            phase_cube.apply_colour_mask(value, mask);
        }
        phase_cube.print();
        std::vector<std::string> solved_states = {};
        if (phase.name == phase7.name) {
            Cube4x4 corner_cube;
            corner_cube.apply_mask(phase7_corners.mask);
            for (const auto& [value, mask] : phase7_corners.colour_mask) {
                corner_cube.apply_colour_mask(value, mask);
            }
            auto table = generate_table({corner_cube.export_state()}, phase7_corners.moves); // (this is 96 length)
            for (const auto& [key, _] : table) {
                solved_states.push_back(key);
            }
        } else {
            solved_states = {phase_cube.export_state()};
        }
        auto table = generate_table(solved_states, phase.moves);
        save_table_binary(table, phase.table_filename);
    }

    return 0;
}