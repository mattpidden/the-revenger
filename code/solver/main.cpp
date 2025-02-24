#include <vector>
#include <limits>
#include <iostream>

#include "cube.h"

// A simple recursive IDA* search function.
static bool ida_search(
    Cube4x4 &cube,
    int depth,
    double &threshold,
    std::vector<Move> &path,
    const std::vector<Move> &all_moves,
    int phase,
    int maxDepth,
    double &next_threshold,
    int &visitedCount,
    int &prunedByF
) {
    visitedCount++;

    // cost so far is 'depth', plus heuristic
    double g = depth;
    double h = cube.twist_distance(phase);  // your "phase_five_twist_distance", etc.
    double f = g + h;

    // if we exceed the current threshold, record for the next iteration
    if (f > threshold) {
        if (f < next_threshold) {
            next_threshold = f;
        }
        prunedByF++;
        return false;
    }

    // if no heuristic cost remains, we’re done
    if (h == 0) {
        return true; 
    }

    // if we've reached our depth limit, stop
    if (depth >= maxDepth) {
        return false;
    }

    // We'll make a copy of the current cube so we can revert easily after each move
    Cube4x4 backup = cube;

    // Try each move
    for (Move mv : all_moves) {
        cube.move(mv);
        path.push_back(mv);

        // Recurse deeper
        if (ida_search(cube, depth + 1, threshold, path, all_moves, phase, maxDepth,
                       next_threshold, visitedCount, prunedByF))
        {
            return true; // found solution
        }

        // Backtrack
        path.pop_back();
        cube = backup; // revert the state
    }

    return false; // no solution at this depth
}

/**
 * A memory-efficient IDA* that only stores one path in recursion.
 *  - @param start_cube is the initial state
 *  - @param phase picks which twist_distance() function you call
 *  - @param all_moves is the list of allowed moves
 *  - @param maxDepth is the maximum depth we’ll allow
 */
std::vector<Move> solve_any_phase_ida(
    const Cube4x4 &start_cube,
    int phase,
    const std::vector<Move> &all_moves,
    int maxDepth
) {
    std::vector<Move> path;
    double threshold = start_cube.twist_distance(phase);

    // stats
    int visitedCount = 0;
    int prunedByF    = 0;

    // Standard iterative deepening loop on 'threshold'
    while (true) {
        double next_threshold = std::numeric_limits<double>::infinity();
        path.clear();

        // We keep a local copy of the cube for the recursion
        Cube4x4 cubeCopy = start_cube;

        bool found = ida_search(
            cubeCopy, 0, threshold, path, all_moves, phase, maxDepth,
            next_threshold, visitedCount, prunedByF
        );

        if (found) {
            // Print some stats and return the path
            std::cout << "[IDA* Stats]\n"
                      << "  Max depth allowed : " << maxDepth   << "\n"
                      << "  Total visited     : " << visitedCount << "\n"
                      << "  Pruned by f       : " << prunedByF    << "\n";
            return path;
        }

        // If we never lowered next_threshold, no solutions remain
        if (next_threshold == std::numeric_limits<double>::infinity()) {
            std::cout << "[IDA* Stats]\n"
                      << "  Max depth allowed : " << maxDepth   << "\n"
                      << "  Total visited     : " << visitedCount << "\n"
                      << "  Pruned by f       : " << prunedByF    << "\n";
            return {};
        }

        // Bump threshold up for the next iteration
        threshold = next_threshold;
    }
}

#include <vector>
#include <limits>
#include <iostream>
#include "cube.h"

/**
 * A depth-limited DFS that returns true if it finds a solution.
 * - @param cube The current cube state (will be mutated, then restored).
 * - @param depth The current recursion depth.
 * - @param limit The maximum depth to explore.
 * - @param path The sequence of moves made so far.
 * - @param all_moves The moves we can apply.
 * - @param phase The phase index to check with twist_distance().
 * - @param visitedCount A counter of visited states for debugging.
 */
static bool dfs_limited(
    Cube4x4 &cube,
    int depth,
    int limit,
    std::vector<Move> &path,
    const std::vector<Move> &all_moves,
    int phase,
    int &visitedCount
) {
    visitedCount++;

    // Check if we're already at the goal
    if (cube.twist_distance(phase) == 0) {
        return true;
    }

    // If we've hit our depth limit, stop
    if (depth >= limit) {
        return false;
    }

    // Save the current state to revert after each move
    Cube4x4 backup = cube;

    // Try each possible move
    for (Move mv : all_moves) {
        cube.move(mv);
        path.push_back(mv);

        // Recurse deeper
        if (dfs_limited(cube, depth + 1, limit, path, all_moves, phase, visitedCount)) {
            return true;  // Found a solution!
        }

        // Backtrack
        path.pop_back();
        cube = backup;
    }

    return false; // No solution found at this depth
}


/**
 * Pure iterative deepening search (no heuristic or cost-based pruning).
 * We search depth 0..maxDepth until we find a path where twist_distance(phase) == 0.
 * This method only stores one path in memory, making it more memory-efficient.
 *
 * @param start_cube  The initial cube configuration
 * @param phase       The phase index; goal check is twist_distance(phase) == 0
 * @param all_moves   The allowed moves at this phase
 * @param maxDepth    The maximum depth to search
 *
 * @return            The sequence of moves leading to a goal (empty if none)
 */
std::vector<Move> solve_any_phase_id(
    const Cube4x4 &start_cube,
    int phase,
    const std::vector<Move> &all_moves,
    int maxDepth
) {
    std::vector<Move> path;
    int visitedCount = 0;

    // For each depth limit from 0 up to maxDepth
    for (int limit = 0; limit <= maxDepth; ++limit) {
        path.clear();

        // Make a copy to mutate during DFS
        Cube4x4 cubeCopy = start_cube;

        // Run a depth-limited DFS
        if (dfs_limited(cubeCopy, 0, limit, path, all_moves, phase, visitedCount)) {
            // Found a solution; print stats
            std::cout << "[IDA Stats]\n"
                      << "  Depth limit      : " << limit       << "\n"
                      << "  Total visited    : " << visitedCount << "\n";
            return path;
        }
    }

    // If we finish all limits with no solution, give up
    std::cout << "[IDA Stats]\n"
              << "  Max depth allowed: " << maxDepth      << "\n"
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

int main() {
    Cube4x4 cube;
    std::vector<Move> scramble = cube.apply_random_moves(35);
    std::cout << "Scramble of size " << scramble.size() << " moves applied: ";
    for (Move move : scramble) {
        std::cout << move_to_string(move) << " ";
    }
    cube.print();
    
    std::cout << "\nSolving Cube...";

    int max_depth = 8;
    std::cout << "\n\nSolving cross... \n";
    std::vector<Move> cross_solution = solve_any_phase_id(cube, 101, { R, L, U, D, F, B }, max_depth);
    print_apply_solution(cube, cross_solution);



    cube.print();

    return 0;
}

