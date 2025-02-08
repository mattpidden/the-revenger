#include <map>
#include <algorithm>
#include <utility>

#include "vector_cube.h"
#include "reduce_centres.h"
#include "reduce_edges.h"
#include "white_cross.h"
#include "first_layer.h"
#include "middle_layer.h"
#include "orient_last_layer.h"
#include "permutate_last_layer.h"

int main() {
    RubiksCube4x4 cube;
    std::vector<std::string> solution = {};

    cube.apply_random_moves(false);
    cube.verify_valid_cube();

    // Reduce centres
    std::vector<std::string> reduce_centres_solution = reduce_centres(cube);
    solution.insert(solution.end(), reduce_centres_solution.begin(), reduce_centres_solution.end());
    cube.display_cube(); 

    // Reduce Edges
    std::vector<std::string> reduce_edges_solution = reduce_edges(cube);
    solution.insert(solution.end(), reduce_edges_solution.begin(), reduce_edges_solution.end());
    cube.display_cube(); 

    // Create white cross on the bottom
    std::vector<std::string> white_cross_solution = solve_white_cross(cube);
    solution.insert(solution.end(), white_cross_solution.begin(), white_cross_solution.end());
    cube.display_cube(); 

    // Solve first layer
    std::vector<std::string> first_layer_solution = solve_first_layer(cube);
    solution.insert(solution.end(), first_layer_solution.begin(), first_layer_solution.end());
    cube.display_cube(); 

    // Solve middle layer
    std::vector<std::string> middle_layer_solution = solve_middle_layer(cube);
    solution.insert(solution.end(), middle_layer_solution.begin(), middle_layer_solution.end());
    cube.display_cube(); 

    // Solve OLL
    std::vector<std::string> orient_last_layer_solution = orient_last_layer(cube);
    solution.insert(solution.end(), orient_last_layer_solution.begin(), orient_last_layer_solution.end());
    cube.display_cube(); 

    // Solve PLL
    std::vector<std::string> permutate_last_layer_solution = permutate_last_layer(cube);
    solution.insert(solution.end(), permutate_last_layer_solution.begin(), permutate_last_layer_solution.end());
    cube.display_cube();

    cube.verify_valid_cube(); 

    std::cout << "Reduce Centres Moves: " << reduce_centres_solution.size() << "\n";
    std::cout << "Reduce Edges Moves: " << reduce_edges_solution.size() << "\n";
    std::cout << "White Cross Moves: " << white_cross_solution.size() << "\n";
    std::cout << "First Layer Moves: " << first_layer_solution.size() << "\n";
    std::cout << "Middle Layer Moves: " << middle_layer_solution.size() << "\n";
    std::cout << "Orient Last Layer Moves: " << orient_last_layer_solution.size() << "\n";
    std::cout << "Permutate Last Layer Moves: " << permutate_last_layer_solution.size() << "\n";
    std::cout << "Total Moves: " << solution.size() << "\n";
    
    return 0;
}
