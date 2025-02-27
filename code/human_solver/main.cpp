#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <utility>
#include <algorithm>

#include "vector_cube.h"
#include "reduce_centres.h"
#include "reduce_edges.h"
#include "white_cross.h"
#include "first_layer.h"
#include "middle_layer.h"
#include "orient_last_layer.h"
#include "permutate_last_layer.h"

static std::pair<double,double> computeMeanStdDev(const std::vector<double>& vals)
{
    if(vals.empty()) {
        return {0.0, 0.0};
    }
    double sum = 0.0;
    for(double v : vals) {
        sum += v;
    }
    double mean = sum / vals.size();

    double sqSum = 0.0;
    for(double v : vals) {
        double diff = v - mean;
        sqSum += diff * diff;
    }
    double stdev = std::sqrt(sqSum / vals.size());
    return {mean, stdev};
}

int main() {
    int n = 100; 
    
    std::vector<double> reduceCentresMoves(n,0.0);
    std::vector<double> reduceEdgesMoves(n,0.0);
    std::vector<double> whiteCrossMoves(n,0.0);
    std::vector<double> firstLayerMoves(n,0.0);
    std::vector<double> middleLayerMoves(n,0.0);
    std::vector<double> orientLastLayerMoves(n,0.0);
    std::vector<double> permutateLastLayerMoves(n,0.0);
    std::vector<double> totalMoves(n,0.0);

    for(int i = 0; i < n; i++) {
        std::cout << "\n--- Trial " << (i+1) << " of " << n << " ---\n";

        RubiksCube4x4 cube;
        cube.apply_random_moves(false); 
        cube.verify_valid_cube();

        std::vector<std::string> solution; // accumulates all moves

        // Reduce centres
        std::vector<std::string> reduce_centres_solution = reduce_centres(cube);
        solution.insert(solution.end(), reduce_centres_solution.begin(), reduce_centres_solution.end());

        // Reduce Edges
        std::vector<std::string> reduce_edges_solution = reduce_edges(cube);
        solution.insert(solution.end(), reduce_edges_solution.begin(), reduce_edges_solution.end());

        // Create white cross on the bottom
        std::vector<std::string> white_cross_solution = solve_white_cross(cube);
        solution.insert(solution.end(), white_cross_solution.begin(), white_cross_solution.end());

        // Solve first layer
        std::vector<std::string> first_layer_solution = solve_first_layer(cube);
        solution.insert(solution.end(), first_layer_solution.begin(), first_layer_solution.end());

        // Solve middle layer
        std::vector<std::string> middle_layer_solution = solve_middle_layer(cube);
        solution.insert(solution.end(), middle_layer_solution.begin(), middle_layer_solution.end());

        // Solve OLL
        std::vector<std::string> orient_last_layer_solution = orient_last_layer(cube);
        solution.insert(solution.end(), orient_last_layer_solution.begin(), orient_last_layer_solution.end());

        // Solve PLL
        std::vector<std::string> permutate_last_layer_solution = permutate_last_layer(cube);
        solution.insert(solution.end(), permutate_last_layer_solution.begin(), permutate_last_layer_solution.end());

        cube.verify_valid_cube(); 

        // Fill out our stats
        reduceCentresMoves[i]       = reduce_centres_solution.size();
        reduceEdgesMoves[i]         = reduce_edges_solution.size();
        whiteCrossMoves[i]          = white_cross_solution.size();
        firstLayerMoves[i]          = first_layer_solution.size();
        middleLayerMoves[i]         = middle_layer_solution.size();
        orientLastLayerMoves[i]     = orient_last_layer_solution.size();
        permutateLastLayerMoves[i]  = permutate_last_layer_solution.size();
        totalMoves[i]               = solution.size();

        std::cout << "   [Trial " << (i+1) << "]\n"
                  << "   Reduce Centres Moves: " << reduce_centres_solution.size() << "\n"
                  << "   Reduce Edges Moves: " << reduce_edges_solution.size() << "\n"
                  << "   White Cross Moves: " << white_cross_solution.size() << "\n"
                  << "   First Layer Moves: " << first_layer_solution.size() << "\n"
                  << "   Middle Layer Moves: " << middle_layer_solution.size() << "\n"
                  << "   Orient Last Layer Moves: " << orient_last_layer_solution.size() << "\n"
                  << "   Permutate Last Layer Moves: " << permutate_last_layer_solution.size() << "\n"
                  << "   Total Moves: " << solution.size() << "\n";
    }

    auto [rcMean, rcStd] = computeMeanStdDev(reduceCentresMoves);
    auto [reMean, reStd] = computeMeanStdDev(reduceEdgesMoves);
    auto [wcMean, wcStd] = computeMeanStdDev(whiteCrossMoves);
    auto [flMean, flStd] = computeMeanStdDev(firstLayerMoves);
    auto [mlMean, mlStd] = computeMeanStdDev(middleLayerMoves);
    auto [ollMean, ollStd] = computeMeanStdDev(orientLastLayerMoves);
    auto [pllMean, pllStd] = computeMeanStdDev(permutateLastLayerMoves);
    auto [totalMean, totalStd] = computeMeanStdDev(totalMoves);

    std::cout << "\n===== AVERAGE RESULTS OVER " << n << " TRIALS =====\n";
    std::cout << "Reduce Centres    : Mean = " << rcMean << ",  Std Dev = " << rcStd << "\n"
              << "Reduce Edges      : Mean = " << reMean << ",  Std Dev = " << reStd << "\n"
              << "White Cross       : Mean = " << wcMean << ",  Std Dev = " << wcStd << "\n"
              << "First Layer       : Mean = " << flMean << ",  Std Dev = " << flStd << "\n"
              << "Middle Layer      : Mean = " << mlMean << ",  Std Dev = " << mlStd << "\n"
              << "Orient LastLayer  : Mean = " << ollMean << ", Std Dev = " << ollStd << "\n"
              << "Permute LastLayer : Mean = " << pllMean << ", Std Dev = " << pllStd << "\n"
              << "Total Moves       : Mean = " << totalMean << ", Std Dev = " << totalStd << "\n";

    return 0;
}
