#include <vector>
#include <limits>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include "cube.h"

// IDA* recursive search function using pruning table
static bool ida_search(Cube4x4 &cube, int depth, double &threshold, std::vector<Move> &path, 
                       const std::vector<Move> &all_moves, int max_depth, double &next_threshold, 
                       const std::unordered_map<std::string, int> &pruning_table, int pruning_depth) {
    double g = depth;
    std::string state = cube.export_state();
    double h = pruning_table.count(state) ? pruning_table.at(state) : pruning_depth + 1;
    double f = g + h;

    if (f > threshold) {
        if (f < next_threshold) {
            next_threshold = f;
        }
        return false;
    }
    if (h == 0) {
        return true;
    }
    if (depth >= max_depth) {
        return false;
    }

    Cube4x4 backup = cube;
    for (Move mv : all_moves) {
        cube.move(mv);
        path.push_back(mv);
        if (ida_search(cube, depth + 1, threshold, path, all_moves, max_depth, next_threshold, pruning_table, pruning_depth)) {
            return true;
        }
        path.pop_back();
        cube = backup; 
    }

    return false;
}

// IDA* solver using pruning table
std::vector<Move> solve_any_phase_ida(const Cube4x4 &start_cube, const std::vector<Move> &all_moves, const std::unordered_map<std::string, int> &pruning_table, int pruning_depth, int max_depth) {
    std::vector<Move> path;
    std::string state = start_cube.export_state();
    
    double threshold = pruning_table.count(state) ? pruning_table.at(state) : pruning_depth + 1;

    while (true) {
        double next_threshold = std::numeric_limits<double>::infinity();
        path.clear();

        Cube4x4 cubeCopy = start_cube;

        bool found = ida_search(cubeCopy, 0, threshold, path, all_moves, max_depth, next_threshold, pruning_table, pruning_depth);
        if (found) {
            return path;
        }
        if (next_threshold == std::numeric_limits<double>::infinity()) {
            return {};
        }

        threshold = next_threshold;
    }
}

// Load the pruning table from a binary file
std::unordered_map<std::string, int> load_table_binary(const std::string& filename) {
    std::unordered_map<std::string, int> table;
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return table;
    }

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

// Print and apply solution to the cube
void print_apply_solution(Cube4x4 &cube, const std::vector<Move> &solution, std::string name) {
    if (!solution.empty()) {
        std::cout << name << " solution found in " << solution.size() << " moves: ";
        for (Move move : solution) {
            std::cout << move_to_string(move) << " ";
            cube.move(move);
        }
        std::cout << std::endl;
    } else {
        std::cout << "No solution found.\n";
    }
}

class Phase {
public:
    std::string name;
    std::vector<Move> moves;
    std::vector<int> mask;
    std::map<int, std::vector<int>> colour_mask;
    std::unordered_map<std::string, int> table;
    int max_depth;

    // Constructor
    Phase(std::string name,
          const std::vector<Move>& moves, 
          const std::vector<int>& mask, 
          const std::map<int, std::vector<int>>& colour_mask, 
          const std::string& table_filename,
          int max_depth)
        : name(name), moves(moves), mask(mask), colour_mask(colour_mask),
          table(load_table_binary(table_filename)), max_depth(max_depth) {}
};


int main() {
    Cube4x4 cube;

    Phase phase1("Phase 1", {R, R_PRIME, R2, L, L_PRIME, L2, F, F_PRIME, F2, B, B_PRIME, B2, U, U_PRIME, U2, D, D_PRIME, D2, r, r_PRIME, r2, l, l_PRIME, l2, f, f_PRIME, f2, b, b_PRIME, b2, u, u_PRIME, u2, d, d_PRIME, d2}, {21,22,25,26,53,54,57,58}, {{-2, {21,22,25,26,53,54,57,58}}}, "phase1table.bin", 8);
    Phase phase2("Phase 2", {R, R_PRIME, R2, L, L_PRIME, L2, F, F_PRIME, F2, B, B_PRIME, B2, U, U_PRIME, U2, D, D_PRIME, D2, r, r_PRIME, r2, l, l_PRIME, l2, f2, b2, u2, d2}, {5,6,9,10,21,22,25,26,37,38,41,42,53,54,57,58,69,70,73,74,85,86,89,90}, {{-3, {5,6,9,10,85,86,89,90}}, {-4, {37,38,41,42,69,70,73,74}}}, "phase2table.bin", 10);

    Phase phase5("Phase 5", {R, R_PRIME, R2, L, L_PRIME, L2, F, F_PRIME, F2, B, B_PRIME, B2, U, U_PRIME, U2, D, D_PRIME, D2}, {1,2,4,7,8,11,13,14,36,39,40,43,68,71,72,75,81,82,84,87,88,91,93,94}, {{-2, {1,2,4,7,8,11,13,14,36,39,40,43,68,71,72,75,81,82,84,87,88,91,93,94}}}, "phase5table.bin", 7);
    Phase phase6("Phase 6", {R, R_PRIME, R2, L, L_PRIME, L2, F2, B2, U, U_PRIME, U2, D, D_PRIME, D2}, {0,1,2,3,4,7,8,11,12,13,14,15,36,39,40,43,68,71,72,75,80,81,82,83,84,87,88,91,92,93,94,95}, {{-3, {0,1,2,3,4,7,8,11,12,13,14,15,80,81,82,83,84,87,88,91,92,93,94,95}}, {-4, {36,39,40,43,68,71,72,75}}}, "phase6table.bin", 10);
    Phase phase7("Phase 7", {R2, L2, F2, B2, U, U_PRIME, U2, D, D_PRIME, D2}, {0,3,12,15,16,17,18,19,20,23,24,27,28,29,30,31,32,33,34,35,36,39,40,43,44,45,46,47,48,49,50,51,52,55,56,59,60,61,62,63,64,65,66,67,68,71,72,75,76,77,78,79,80,83,92,95}, {{-3, {17,18,20,23,24,27,29,30,49,50,52,55,56,59,61,62}}, {-4, {33,34,36,39,40,43,45,46,65,66,68,71,72,75,77,78}}}, "phase7table.bin", 13);
    Phase phase8("Phase 8", {R2, L2, F2, B2, U2, D2}, {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95}, {}, "phase8table.bin", 15);
    std::array<Phase, 2> phases = {phase1, phase2};
 
    //cube.import_state("WYYWYWWWYWWWWYYWOOOOOOOROOORRRRRGGGGGGGBGGGBBBBBROORRRRORRROORROBBBBBBBGBBBGGGGGYWWYWYYYWYYYYWWY");
    std::vector<Move> scramble = cube.apply_random_moves(35, {R, L, U, D, F, B, r, l, u, d, f, b});
    std::cout << "Scramble of size " << scramble.size() << " moves: ";
    for (Move move : scramble) {
        std::cout << move_to_string(move) << " ";
        cube.move(move);
    }
    Cube4x4 scrambled_cube = cube;
    cube.print();

    std::vector<Move> solution = {};
    for (const Phase& phase : phases) {
        Cube4x4 phase_cube = cube;
        phase_cube.apply_mask(phase.mask);
        for (const auto& [value, mask] : phase.colour_mask) {
            phase_cube.apply_colour_mask(value, mask);
        }
        std::vector<Move> phase_solution = solve_any_phase_ida(phase_cube, phase.moves, phase.table, phase.max_depth, phase.max_depth);
        for (const Move& move : phase_solution) {
            solution.push_back(move);
        }
        print_apply_solution(cube, phase_solution, phase.name);
    }

    print_apply_solution(scrambled_cube, solution, "Full");
    scrambled_cube.print();
    return 0;
}
