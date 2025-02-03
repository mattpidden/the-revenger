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

#include "permutate_last_layer.h"
#include "vector_cube.h"

std::vector<std::string> permutate_last_layer(RubiksCube4x4& cube) { 
    std::vector<std::string> solution = {};
    
    // 1. rotate until pll look 1 algo is applicable, then perform it
    std::map<std::vector<std::vector<int>>, std::vector<std::string>> pll_look_one = {
        {{{64,83}, {67,80}, {48,35}, {51,32}}, {"F","R","U'","R'","U'","R","U","R'", "F'","R","U","R'","U'", "R'","F", "R", "F'"}},
        {{{51,32}, {67,64}, {48,83}, {80,35}}, {"R","U","R'","U'","R'","F","R2", "U'","R'","U'","R","U", "R'","F'"}},
        {{{64,67}, {48,51}, {80,83}, {32,35}}, {}}}; 
    
    while (true) {
        bool move_applied = false;
    
        for (const auto& [indexes, moves] : pll_look_one) {
            if (cube.check_indexes_match_indexes(indexes)) {  
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

    while (!cube.check_corners_aligned_pll()) {
        cube.apply_move("U");
        solution.push_back("U");
    }

    // 2. check for parity
    std::vector<std::vector<std::vector<int>>> edges = cube.check_edge_centre_alignment();
    std::vector<std::vector<int>> aligned_edges = edges[0];
    std::vector<std::vector<int>> unaligned_edges = edges[1];

    if (aligned_edges.size() == 2) {
        std::vector<std::string> move = cube.rotate_cube_so_piece_on_face(unaligned_edges[0][0], 'F');
        solution.insert(solution.end(), move.begin(), move.end());
        cube.apply_moves(move);
        std::vector<std::string> parity_algo = {"r2", "U2", "r2", "Uw2", "r2", "u2"};
        solution.insert(solution.end(), parity_algo.begin(), parity_algo.end());
        cube.apply_moves(parity_algo);
    }

    // 3. finish with pll look two
    std::map<std::vector<std::vector<int>>, std::vector<std::string>> pll_look_two = {
        {{{65,66,80,83}, {64,67,81,82}, {48,51,33,34}, {49,50,32,35}}, {"R2","U2","R","U2","R2","U2","R2","U2","R","U2","R2"}},
        {{{49,50,80,83}, {64,67,81,82}, {65,66,48,51}, {32,33,34,35}}, {"R","U'","R","U","R","U","R", "U'","R'","U'","R2"}},
        {{{65,66,80,83}, {49,50,64,67}, {48,51,81,82}, {32,33,34,35}}, {"R2","U","R","U","R'","U'","R'","U'","R'","U","R'"}},
        {{{81,82,32,35}, {65,66,48,51}, {33,34,80,83}, {49,50,64,67}}, {"R'","U'","R","U'","R","U","R","U'","R'","U","R","U","R2","U'","R'","U"}},
        {{{64,65,66,67}, {48,48,50,51}, {80,81,82,83}, {32,33,34,35}}, {}}}; 
    
    int count = 0;
    while (true) {
        bool move_applied = false;
    
        for (const auto& [indexes, moves] : pll_look_two) {
            if (cube.check_indexes_match_indexes(indexes)) {  
                cube.apply_moves(moves);
                solution.insert(solution.end(), moves.begin(), moves.end());
                move_applied = true;
                break;
            }
        }

        if (!move_applied) {
            cube.apply_move("y");
            solution.push_back("y");
        } else {
            break;
        }
        count = count + 1;
        if (count > 4) {
            std::vector<std::string> parity_algo = {"r2", "U2", "r2", "Uw2", "r2", "u2"};
            solution.insert(solution.end(), parity_algo.begin(), parity_algo.end());
            cube.apply_moves(parity_algo);
            count = 0;
        }
    }

    while (!cube.check_corners_aligned_pll()) {
        cube.apply_move("U");
        solution.push_back("U");
    }
    
    return solution;
}