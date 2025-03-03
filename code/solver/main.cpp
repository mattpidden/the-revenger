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
void print_apply_solution(Cube4x4 &cube, const std::vector<Move> &solution) {
    if (!solution.empty()) {
        std::cout << "Solution found in " << solution.size() << " moves: ";
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
    std::vector<Move> moves;
    std::vector<int> mask;
    bool apply_no_colour;
    std::unordered_map<std::string, int> table;
    int table_depth;
    int max_depth;

    // Constructor
    Phase(const std::vector<Move>& moves, 
          const std::vector<int>& mask, 
          bool apply_no_colour, 
          const std::string& table_filename, 
          int table_depth, 
          int max_depth)
        : moves(moves), mask(mask), apply_no_colour(apply_no_colour), 
          table(load_table_binary(table_filename)), 
          table_depth(table_depth), max_depth(max_depth) {}
};


int main() {
    Cube4x4 cube;

    Phase phase5({R, L, F, B, U, D}, {1,2,4,7,8,11,13,14,36,39,40,43,68,71,72,75,81,82,84,87,88,91,93,94}, true, "phase5table.bin", 10, 15);
    
    Phase phase8({R2, L2, F2, B2, U2, D2}, {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95}, false, "phase8table.bin", 14, 25);

    std::array<Phase, 1> phases = {phase5};
 
    //cube.import_state("WYYWYWWWYWWWWYYWOOOOOOOROOORRRRRGGGGGGGBGGGBBBBBROORRRRORRROORROBBBBBBBGBBBGGGGGYWWYWYYYWYYYYWWY");
    std::vector<Move> scramble = cube.apply_random_moves(35, {R, L, U, D, F, B});
    std::cout << "Scramble of size " << scramble.size() << " moves: ";
    for (Move move : scramble) {
        std::cout << move_to_string(move) << " ";
        cube.move(move);
    }
    cube.print();

    std::vector<Move> solution = {};
    for (const Phase& phase : phases) {
        Cube4x4 phase_cube = cube;
        phase_cube.apply_mask(phase.mask);
        if (phase.apply_no_colour) {
            phase_cube.apply_no_colour();
        }
        std::vector<Move> phase_solution = solve_any_phase_ida(phase_cube, phase.moves, phase.table, phase.table_depth, phase.max_depth);
        for (const Move& move : phase_solution) {
            solution.push_back(move);
        }
        print_apply_solution(cube, phase_solution);
    }
    
    cube.print();
    return 0;
}
