#include <map>
#include <algorithm>
#include <utility>

#include "vector_cube.h"
#include "reduce_centres.h"
#include "reduce_edges.h"
#include "white_cross.h"
#include "first_layer.h"
#include "middle_layer.h"

int main() {
    RubiksCube4x4 cube;
    std::vector<std::string> solution = {};

    cube.apply_random_moves(false);
    cube.verify_valid_cube();

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
    // 1. get list of yellow edge bars on top, and not on top
    cube.display_cube();
    std::vector<std::vector<std::vector<std::vector<int>>>> yellow_edges = cube.yellow_edge_bars_on_top();
    std::vector<std::vector<std::vector<int>>> top_edges = yellow_edges[0];
    std::vector<std::vector<std::vector<int>>> non_top_edges = yellow_edges[1];
    std::vector<std::string> parity_algo = {"r2", "B2", "U2", "l", "U2", "r'", "U2", "r", "U2", "F2", "r", "F2", "l'", "B2", "r2"};

    if (top_edges.size() == 1) {
        int edge_to_move_back = top_edges[0][1][0];
        std::vector<std::string> move = cube.rotate_cube_so_piece_on_face(edge_to_move_back, 'B');
        solution.insert(solution.end(), move.begin(), move.end());
        cube.apply_moves(move);
        solution.insert(solution.end(), parity_algo.begin(), parity_algo.end());
        cube.apply_moves(parity_algo);
        std::cout << "Applied Parity Algo" << "\n";
        cube.display_cube();
    } else if (top_edges.size() == 3) {
        int edge_to_move_back = non_top_edges[0][1][0];
        std::vector<std::string> move = cube.rotate_cube_so_piece_on_face(edge_to_move_back, 'F');
        solution.insert(solution.end(), move.begin(), move.end());
        cube.apply_moves(move);
        solution.insert(solution.end(), parity_algo.begin(), parity_algo.end());
        cube.apply_moves(parity_algo);
        std::cout << "Applied Parity Algo" << "\n";
        cube.display_cube();
    }
    


    // 3. rotate until oll 1 algo is applicable, then perform it
    
    
    std::map<std::vector<int>, std::vector<std::string>> oll_look_one = {
        {{5,6,9,10,49,50,81,82,33,34,65,66}, {"F","R","U","R'","U'","F'","f","R","U","R'","U'","f'"}},
        {{4,5,6,7,8,9,10,11,49,50,33,34}, {"F","R","U","R'","U'","F'"}},
        {{5,6,9,10,7,11,13,14,33,34,65,66}, {"f","R","U","R'","U'","f'"}}}; // These 'f' moves are for 3x3 not 4x4 need to fix
    
    int count = 1;
    while (true) {
        bool move_applied = false;
    
        for (const auto& [indexes, moves] : oll_look_one) {
            if (cube.check_indexes_match_colour(indexes, 'Y')) {  
                cube.apply_moves(moves);
                move_applied = true;
                break;
            }
        }

        if (!move_applied) {
            cube.apply_move("U");
            cube.display_cube();
        } else {
            break;
        }
        if (count > 4) {
            break;
        }
        count = count + 1;
    }

    // 4. rotate until oll2 algo is applicable, then perform it

    cube.display_cube();    
    std::cout << "Total Moves: " << solution.size() << "\n";
    cube.verify_valid_cube();

    return 0;
}
