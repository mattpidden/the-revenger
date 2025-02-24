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
    std::vector<Move> cross_solution = solve_any_phase_ida(cube, 101, { R, L, U, D, F, B }, max_depth);
    print_apply_solution(cube, cross_solution);



    cube.print();

    return 0;
}

