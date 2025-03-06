#include <vector>
#include <limits>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <functional>
#include <chrono>
#include "cube.h"

// IDA* recursive search function using pruning table
static bool ida_search(const Phase &phase, Cube4x4 &cube, int depth, double &threshold, std::vector<Move> &path, const std::vector<Move> &all_moves, int max_depth, 
                                                    double &next_threshold, const std::unordered_map<std::string, int> &pruning_table, int pruning_depth) {
    if (phase.is_solved(cube)) {
        return true;
    }
    std::string state = cube.export_state();
    double h = pruning_table.count(state) ? pruning_table.at(state) : pruning_depth + 1;
    double f = depth + h;

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
        if (ida_search(phase, cube, depth + 1, threshold, path, all_moves, max_depth, next_threshold, pruning_table, pruning_depth)) {
            return true;
        }
        path.pop_back();
        cube = backup; 
    }

    return false;
}

// IDA* solver using pruning table
std::vector<Move> solve_any_phase_ida(const Phase &phase, Cube4x4 &start_cube, const std::vector<Move> &all_moves, 
                                      const std::unordered_map<std::string, int> &pruning_table, int pruning_depth, int max_depth) {
    std::vector<Move> path;
    std::string state = phase.mask(start_cube);
    
    double threshold = pruning_table.count(state) ? pruning_table.at(state) : pruning_depth + 1;

    while (true) {
        double next_threshold = std::numeric_limits<double>::infinity();
        path.clear();

        Cube4x4 cubeCopy = start_cube;

        bool found = ida_search(phase, cubeCopy, 0, threshold, path, all_moves, max_depth, next_threshold, pruning_table, pruning_depth);
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


int main() {
    Cube4x4 cube;

    std::vector<Phase> phases = {phase5, phase6, phase7, phase8};
    for (Phase& phase : phases) {
        phase.set_table(load_table_binary(phase.table_filename));
    }

    //cube.import_state("WYYWYWWWYWWWWYYWOOOOOOOROOORRRRRGGGGGGGBGGGBBBBBROORRRRORRROORROBBBBBBBGBBBGGGGGYWWYWYYYWYYYYWWY");
    std::vector<Move> scramble = cube.apply_random_moves(35, {R, L, U, D, F, B});
    std::cout << "Scramble of size " << scramble.size() << " moves: ";
    for (Move move : scramble) {
        std::cout << move_to_string(move) << " ";
        cube.move(move);
    }
    Cube4x4 scrambled_cube = cube;
    cube.print();

    auto start = std::chrono::high_resolution_clock::now();
    std::vector<Move> solution = {};
    for (const Phase& phase : phases) {
        Cube4x4 phase_cube = cube;
        
        std::vector<Move> phase_solution = solve_any_phase_ida(phase, phase_cube, phase.moves, phase.table, phase.table_depth_limit, phase.search_depth_limit);
        for (const Move& move : phase_solution) {
            solution.push_back(move);
        }
        print_apply_solution(cube, phase_solution, phase.name);
        cube.print();
    }
    auto end = std::chrono::high_resolution_clock::now();

    print_apply_solution(scrambled_cube, solution, "Full");
    std::cout << "Execution time: " << std::chrono::duration<double>(end - start).count() << " seconds\n";

    return 0;
}
