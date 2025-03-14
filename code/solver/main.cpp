#include <vector>
#include <limits>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <functional>
#include <chrono>
#include "cube.h"
#include "BooPHF.h"

using namespace boomphf;

// IDA* recursive search function using table
static bool ida_search(Phase &phase, Cube4x4 &cube, int depth, double &threshold, std::vector<Move> &path, double &next_threshold) {
    std::string state = phase.mask(cube);
    uint64_t idx = phase.hash_table.lookup(state);

    double h = (idx >= phase.depths.size()) ? (phase.table_depth_limit + 1) : static_cast<double>(static_cast<unsigned char>(phase.depths[idx]));

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
    if (depth >= phase.search_depth_limit) {
        return false;
    }

    Cube4x4 backup = cube;
    for (Move mv : phase.moves) {
        cube.move(mv);
        path.push_back(mv);

        if (ida_search(phase, cube, depth + 1, threshold, path, next_threshold)) {
            return true;
        }
        path.pop_back();
        cube = backup;
    }

    return false;
}

std::vector<Move> solve_any_phase_ida(Phase &phase, Cube4x4 start_cube) {
    std::vector<Move> path;
    std::string state = phase.mask(start_cube);
    uint64_t idx = phase.hash_table.lookup(state);

    double threshold = (idx >= phase.depths.size()) ? (phase.table_depth_limit + 1) : static_cast<double>(static_cast<unsigned char>(phase.depths[idx]));

    while (true) {
        double next_threshold = std::numeric_limits<double>::infinity();
        path.clear();

        bool found = ida_search(phase, start_cube, 0, threshold, path, next_threshold);
        if (found) {
            return path;
        }
        if (next_threshold == std::numeric_limits<double>::infinity()) {
            return {};
        }
        threshold = next_threshold;
    }
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
        std::cout << "No solution found." << std::endl;
    }
}

std::vector<char> load_depths(const std::string &filename) {
    std::ifstream in(filename, std::ios::binary);
    uint64_t sz = 0;
    in.read(reinterpret_cast<char*>(&sz), sizeof(sz));
    std::vector<char> depths(sz);
    in.read(reinterpret_cast<char*>(depths.data()), sz);
    return depths;
}

boomphf::mphf<std::string, MyStringHash> load_hash_table(const std::string &filename) {
    boomphf::mphf<std::string, MyStringHash> mph;
    std::ifstream ifs(filename, std::ios::binary);
    mph.load(ifs);
    return mph;
}

int lookup_depth(const std::string &state, const std::string &mph_file, const std::string &depth_file) {
    auto mph = new boomphf::mphf<std::string, MyStringHash>();
    std::ifstream ifs(mph_file, std::ios::binary);
    mph->load(ifs);
    auto depths = load_depths(depth_file);
    uint64_t idx = mph->lookup(state);
    delete mph;
    if (idx >= depths.size()) {
        return -1;
    }
    return depths[idx];
}


int main() {
    // Create cube and scramble it
    Cube4x4 cube;
    std::vector<Move> scramble = cube.apply_random_moves(35, {R,L,F,B,U,D,r,l,f,b,u,d});
    std::cout << "Scramble of size " << scramble.size() << " moves: ";
    for (Move move : scramble) {
        std::cout << move_to_string(move) << " ";
    }
    Cube4x4 scrambled_cube = cube;
    cube.print();

    // Load the data in
    std::vector<Phase> phases = {phase1, phase2a, phase2, phase3, phase4, phase5, phase6, phase7, phase8};
    for (Phase &phase : phases) {
        std::cout << "Loading " << phase.name << " table..." << std::endl;;
        std::string mphFile = phase.table_filename + ".mph";
        std::string depthFile = phase.table_filename + ".depths";
        auto mph = load_hash_table(mphFile);
        auto dvec = load_depths(depthFile);
        phase.set_hash_table(mph);
        phase.set_depths(dvec);
    }

    // Solve the cube
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<Move> solution = {};
    for (Phase &phase : phases) {
        std::vector<Move> phase_solution = solve_any_phase_ida(phase, cube);
        solution.insert(solution.end(), phase_solution.begin(), phase_solution.end());
        print_apply_solution(cube, phase_solution, phase.name);
        cube.print();
    }
    auto end = std::chrono::high_resolution_clock::now();
    print_apply_solution(scrambled_cube, solution, "Full");
    std::cout << "Solution time: " << std::chrono::duration<double>(end - start).count() << " seconds." << std::endl;

    return 0;
}