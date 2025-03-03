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
std::vector<Move> solve_any_phase_ida(const Cube4x4 &start_cube, const std::vector<Move> &all_moves, 
                                      int max_depth, const std::unordered_map<std::string, int> &pruning_table, int pruning_depth) {
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

int main() {
    Cube4x4 cube;
    std::vector<Move> phase8_moves = {R2, L2, F2, B2, U2, D2};
    //cube.import_state("WYYWYWWWYWWWWYYWOOOOOOOROOORRRRRGGGGGGGBGGGBBBBBROORRRRORRROORROBBBBBBBGBBBGGGGGYWWYWYYYWYYYYWWY");
    
    std::vector<Move> scramble = cube.apply_random_moves(35, phase8_moves);
    std::cout << "Scramble of size " << scramble.size() << " moves: ";
    for (Move move : scramble) {
        std::cout << move_to_string(move) << " ";
        cube.move(move);
    }
    cube.print();

    std::unordered_map<std::string, int> pruning_table = load_table_binary("phase8table.bin");
    int pruning_depth = 14;

    std::vector<Move> solution = solve_any_phase_ida(cube, phase8_moves, 25, pruning_table, pruning_depth);

    print_apply_solution(cube, solution);
    return 0;
}
