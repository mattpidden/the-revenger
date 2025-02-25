#include <vector>
#include <limits>
#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>
#include <cmath>
#include "cube.h"

static std::atomic<bool> found_solution{false};

// Recursive IDA* search function.
static bool ida_search(Cube4x4 &cube, int depth, double &threshold, std::vector<Move> &path, const std::vector<Move> &all_moves, int phase, int max_depth, double &next_threshold, int &visitedCount, int &prunedByF) {
    visitedCount++;

    double g = depth;
    double h = cube.twist_distance(phase); 
    double f = g + h;

    if (f > threshold) {
        if (f < next_threshold) {
            next_threshold = f;
        }
        prunedByF++;
        return false;
    }

    if (h == 0) {
        return true; 
    }

    if (depth >= max_depth) {
        return false;
    }
    if (found_solution.load(std::memory_order_relaxed)) {
        return false;
    }

    Cube4x4 backup = cube;

    for (Move mv : all_moves) {
        cube.move(mv);
        path.push_back(mv);

        if (ida_search(cube, depth + 1, threshold, path, all_moves, phase, max_depth,
                       next_threshold, visitedCount, prunedByF))
        {
            return true; 
        }

        // Backtrack
        path.pop_back();
        cube = backup; 
    }

    return false; 
}

// IDA* that only stores one path in recursion
std::vector<Move> solve_any_phase_ida(const Cube4x4 &start_cube, int phase, const std::vector<Move> &all_moves, int max_depth) {
    std::vector<Move> path;
    double threshold = start_cube.twist_distance(phase);

    int visitedCount = 0;
    int prunedByF    = 0;

    while (true) {
        double next_threshold = std::numeric_limits<double>::infinity();
        path.clear();

        Cube4x4 cubeCopy = start_cube;

        bool found = ida_search(
            cubeCopy, 0, threshold, path, all_moves, phase, max_depth,
            next_threshold, visitedCount, prunedByF
        );

        if (found) {
            std::cout << "[IDA* Stats]\n"
                      << "  Max depth allowed : " << max_depth   << "\n"
                      << "  Total visited     : " << visitedCount << "\n"
                      << "  Pruned by f       : " << prunedByF    << "\n";
            return path;
        }

        if (next_threshold == std::numeric_limits<double>::infinity()) {
            std::cout << "[IDA* Stats]\n"
                      << "  Max depth allowed : " << max_depth   << "\n"
                      << "  Total visited     : " << visitedCount << "\n"
                      << "  Pruned by f       : " << prunedByF    << "\n";
            return {};
        }

        threshold = next_threshold;
    }
}


// Depth-limited DFS
static bool dfs_limited(Cube4x4 &cube, int depth, int limit, std::vector<Move> &path, const std::vector<Move> &all_moves, int phase, int &visitedCount) {
    visitedCount++;

    if (cube.twist_distance(phase) == 0) {
        return true;
    }

    if (depth >= limit) {
        return false;
    }
    if (found_solution.load(std::memory_order_relaxed)) {
        return false;
    }
    Cube4x4 backup = cube;

    for (Move mv : all_moves) {
        cube.move(mv);
        path.push_back(mv);

        if (dfs_limited(cube, depth + 1, limit, path, all_moves, phase, visitedCount)) {
            return true;  
        }

        path.pop_back();
        cube = backup;
    }

    return false;
}


// Iterative deepening search (no heuristic)
std::vector<Move> solve_any_phase_id(const Cube4x4 &start_cube, int phase, const std::vector<Move> &all_moves, int max_depth) {
    std::vector<Move> path;
    int visitedCount = 0;

    for (int limit = 0; limit <= max_depth; ++limit) {
        path.clear();

        Cube4x4 cubeCopy = start_cube;

        if (dfs_limited(cubeCopy, 0, limit, path, all_moves, phase, visitedCount)) {
            std::cout << "[IDA Stats]\n"
                      << "  Depth limit      : " << limit       << "\n"
                      << "  Total visited    : " << visitedCount << "\n";
            return path;
        }
    }

    std::cout << "[IDA Stats]\n"
              << "  Max depth allowed: " << max_depth      << "\n"
              << "  Total visited    : " << visitedCount   << "\n";
    return {};
}


void print_apply_solution(Cube4x4 &cube, std::vector<Move> solution) {
    if (!solution.empty()) {
        std::cout << "Solution found in " << solution.size() << " moves: ";
        for (Move move : solution) {
            std::cout << move_to_string(move) << " ";
            cube.move(move);
        }
    } else {
        std::cout << "No solution found. \n";
    }
}

struct ThreadResult {
    std::vector<Move> solution;
    bool found_something;
    int index;
};

// This function runs in each thread
void parallel_solve_move_set(const Cube4x4 &original, int phase, std::vector<Move> move_set, int max_depth, ThreadResult &result) {
    Cube4x4 localCopy = original;
    
    std::vector<Move> sol = solve_any_phase_ida(localCopy, phase, move_set, max_depth);
    if (!sol.empty()) {
        result.solution = sol;
        result.found_something = true;
        found_solution.store(true, std::memory_order_relaxed);
    }

    result.solution      = sol;
    result.found_something = !sol.empty();
}


std::vector<Move> run_parallel_phase(Cube4x4 &cube, int phase, const std::vector<std::vector<Move>> &move_sets, int max_depth) {
    std::cout << "\n\n==== Starting parallel search for phase " << phase << " ====\n";

    std::vector<ThreadResult> results(move_sets.size());
    std::vector<std::thread> threads;
    threads.reserve(move_sets.size());

    found_solution.store(false, std::memory_order_relaxed);
    for (int i = 0; i < (int)move_sets.size(); i++) {
        results[i].index = i;
        threads.emplace_back(parallel_solve_move_set,
                             std::cref(cube),
                             phase,
                             move_sets[i],
                             max_depth,
                             std::ref(results[i]) );
    }

    for (auto &t : threads) {
        t.join();
    }

    int best_index = -1;
    size_t bestSize = std::numeric_limits<size_t>::max();

    for (auto &r : results) {
        if (r.found_something && r.solution.size() < bestSize) {
            best_index = r.index;
            bestSize  = r.solution.size();
        }
    }

    if (best_index >= 0) {
        std::cout << "\nThread " << best_index
                  << " found a solution with " << bestSize << " moves:\n";

        for (auto mv : results[best_index].solution) {
            std::cout << move_to_string(mv) << " ";
            cube.move(mv);
        }
        std::cout << "\n";
        return results[best_index].solution;
    } else {
        std::cout << "\nNo solutions found at depth <= " << max_depth << "\n";
        return {}; 
    }
}

std::pair<double,double> compute_mean_std_dev(const std::vector<int> &values) {
    if (values.empty()) return {0.0, 0.0};

    double sum = 0.0;
    for (int v : values) {
        sum += v;
    }
    double mean = sum / values.size();

    double sqSum = 0.0;
    for (int v : values) {
        double diff = v - mean;
        sqSum += diff * diff;
    }
    double variance = sqSum / values.size();
    double stdev = std::sqrt(variance);

    return {mean, stdev};
}


int main() {
    const int numTrials = 100;
    std::vector<int> total_lengths;
    total_lengths.reserve(numTrials);

    for (int trial = 1; trial <= numTrials; trial++) {
        std::cout << "\n=== Trial " << trial << " of " << numTrials << " ===\n";

        Cube4x4 cube;
        std::vector<Move> scramble = cube.apply_random_moves(35);
        std::cout << "Scramble of size " << scramble.size() << " moves applied.\n";

        int max_depth = 9;
        std::vector<std::vector<Move>> move_sets_1 = {
            {r, l, u, d, f, b},
            {r, l, u, d, f, b, r_PRIME},
            {r, l, u, d, f, b, l_PRIME},
            {r, l, u, d, f, b, f_PRIME},
            {r, l, u, d, f, b, b_PRIME},
            {r, l, u, d, f, b, u_PRIME},
            {r, l, u, d, f, b, d_PRIME},
            {r, l, u, d, f, b, F},
            {r, l, u, d, f, b, B},
            {r, l, u, d, f, b, R},
            {r, l, u, d, f, b, L},
            {r, l, u, d, f, b, U},
            {r, l, u, d, f, b, D},
            {b, f, u, d, l, r, U},
        };
        auto solution101 = run_parallel_phase(cube, 101, move_sets_1, max_depth);

        std::vector<std::vector<Move>> move_sets_2 = {
            {r, l, u, f, b},
            {r, l,  f, b, r_PRIME},
            {r, l, f, b, l_PRIME},
            {r, l, f, b, f_PRIME},
            {r, l, f, b, b_PRIME},
            {r, l, f, b, u_PRIME},
            {r, l,  f, b, d_PRIME},
            {r, l,  f, b, F},
            {r, l,  f, b, B},
            {r, l, f, b, R},
            {r, l, f, b, L},
            {r, l, f, b, U},
            {r, l,  f, b, D},
            {b, f, l, r, U},
        };
        auto solution102 = run_parallel_phase(cube, 102, move_sets_2, max_depth);
        auto solution103 = run_parallel_phase(cube, 103, move_sets_2, max_depth);

        std::vector<std::vector<Move>> move_sets_4 = {
            {u, d, F, B, L, R},
            {u, d, F, B, L, R, u_PRIME},
            {u, d, F, B, L, R, d_PRIME},
            {u, d, F, B, L, R, F_PRIME},
            {R, L, u, d, F, B, B_PRIME},
            {F, B, L, R, u, d, L_PRIME},
            {u, d, F, B, L, R, R_PRIME},
        };
        auto solution104 = run_parallel_phase(cube, 104, move_sets_4, max_depth);
        auto solution105 = run_parallel_phase(cube, 105, move_sets_4, max_depth);

        std::vector<std::vector<Move>> move_sets_6 = {
            {u, d, F, B, r, l},
            {u, d, F, B, u_PRIME},
            {u, d, F, B, d_PRIME},
            {u, d, F, B, r_PRIME},
            {u, d, F, B, l_PRIME},
            {u, d, F, B, l_PRIME},
            {u, d, F, B, r_PRIME},
        };
        auto solution106 = run_parallel_phase(cube, 106, move_sets_6, max_depth);
        if (cube.blue_green_centre_twist_distance() == 0) {
            int length = (int)solution101.size() + (int)solution102.size() +
                        (int)solution103.size() + (int)solution104.size() +
                        (int)solution105.size() + (int)solution106.size();

                        cube.print();

            std::cout << "Total solution length for this trial = " << length << "\n";
            total_lengths.push_back(length);
        }

        
    }

    auto [mean, stdev] = compute_mean_std_dev(total_lengths);

    std::cout << "\n========================================\n";
    std::cout << "Over " << numTrials << " trials:\n";
    std::cout << "  Average total solution length: " << mean << "\n";
    std::cout << "  Std Dev: " << stdev << "\n";
    std::cout << "========================================\n";

    return 0;
}
