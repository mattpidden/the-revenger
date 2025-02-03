#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <sstream> 
#include <random>
#include <unordered_set>
#include <algorithm>
#include <utility>

#include "orient_last_layer.h"
#include "vector_cube.h"

std::vector<std::string> orient_last_layer(RubiksCube4x4& cube) { 
    std::vector<std::string> solution = {};
    
    // 1. get list of yellow edge bars on top, and not on top
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
    } else if (top_edges.size() == 3) {
        int edge_to_move_back = non_top_edges[0][1][0];
        std::vector<std::string> move = cube.rotate_cube_so_piece_on_face(edge_to_move_back, 'F');
        solution.insert(solution.end(), move.begin(), move.end());
        cube.apply_moves(move);
        solution.insert(solution.end(), parity_algo.begin(), parity_algo.end());
        cube.apply_moves(parity_algo);
        std::cout << "Applied Parity Algo" << "\n";
    }

    // 3. rotate until oll 1 algo is applicable, then perform it
    std::map<std::vector<int>, std::vector<std::string>> oll_look_one = {
        {{5,6,9,10,49,50,81,82,33,34,65,66}, {"F","R","U","R'","U'","F'","B","z", "R","U","R'","U'","B'", "z'"}},
        {{4,5,6,7,8,9,10,11,49,50,33,34}, {"F","R","U","R'","U'","F'"}},
        {{5,6,9,10,7,11,13,14,33,34,65,66}, {"B", "z","R","U","R'","U'","B'", "z'"}},
        {{1,2,4,5,6,7,8,9,10,11,13,14}, {}}}; 
    
    while (true) {
        bool move_applied = false;
    
        for (const auto& [indexes, moves] : oll_look_one) {
            if (cube.check_indexes_match_colour(indexes, 'Y')) {  
                cube.apply_moves(moves);
                solution.insert(solution.end(), moves.begin(), moves.end());
                move_applied = true;
                break;
            }
        }

        if (!move_applied) {
            cube.apply_move("U");
            solution.push_back("U");
        } else {
            break;
        }
    }

    // 4. rotate until oll2 algo is applicable, then perform it
    std::map<std::vector<int>, std::vector<std::string>> oll_look_two = {
        {{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15}, {}},
        {{1,2,3,4,5,6,7,8,9,10,11,13,14,48,80,64}, {"R","U2","R'","U'","R","U'","R'"}},
        {{1,2,4,5,6,7,8,9,10,11,13,14,64,67,80,83}, {"R","U","R'","U","R","U'","R'","U","R","U2","R'"}},
        {{0,1,2,4,5,6,7,8,9,10,11,13,14,15,48,83}, {"F","R'","F'","L","x","U","R","U'","L'","x"}},
        {{1,2,4,5,6,7,8,9,10,11,13,14,51,32,64,67}, {"R","U2","R2","U'","R2","U'","R2","U2","R"}},
        {{1,2,4,5,6,7,8,9,10,11,12,13,14,51,83,35}, {"R","U","R'","U","R","U2","R'"}},
        {{1,2,3,4,5,6,7,8,9,10,11,13,14,15,48,35}, {"L","x", "U", "R'", "U'", "L'","x'", "F", "R", "F'"}},
        {{0,1,2,3,4,5,6,7,8,9,10,11,13,14,48,51}, {"R2","D","R'","U2","R","D'","R'","U2","R'"}}}; 

    while (true) {
        bool move_applied = false;
    
        for (const auto& [indexes, moves] : oll_look_two) {
            if (cube.check_indexes_match_colour(indexes, 'Y')) {  
                cube.apply_moves(moves);
                solution.insert(solution.end(), moves.begin(), moves.end());
                move_applied = true;
                break;
            }
        }

        if (!move_applied) {
            cube.apply_move("U");
            solution.push_back("U");
        } else {
            break;
        }
    }

    std::vector<int> white_centre_pieces = cube.find_centre_pieces('W');
    std::vector<std::string> move = cube.rotate_cube_so_piece_on_face(white_centre_pieces[0], 'D');
    solution.insert(solution.end(), move.begin(), move.end());
    cube.apply_moves(move);

    return solution;
}