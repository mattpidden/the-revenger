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

#include "first_layer.h"
#include "vector_cube.h"

std::vector<std::string> solve_first_layer(RubiksCube4x4& cube) { 
    std::vector<std::string> solution = {};
    
    std::map<std::vector<int>, std::vector<std::string>> corner_to_front_bottom_white_down = {{{15,51,80}, {}}, {{3,83,32}, {"U"}}, {{0,64,35}, {"U2"}}, {{12,48,67}, {"U'"}}, {{63,92,19}, {"R", "U", "R'", "U'"}}, {{60,79,16}, {"L'", "U'", "L"}}, {{95,44,31}, {"R'", "U'", "R", "U2"}}, {{76,47,28}, {"L", "U2", "L'"}}};
    
    std::vector<char> colours = {'G', 'O', 'B', 'R'};
    std::vector<std::string> four_move_algo = {"R", "U", "R'", "U'"};

    for (size_t i = 0; i < colours.size(); i++) {
        char colour1 = colours[i];
        char colour2 = colours[(i + 1) % colours.size()];

        std::vector<int> colour_centre_pieces = cube.find_centre_pieces(colour1);
        std::vector<std::string> move = cube.rotate_cube_so_piece_on_face(colour_centre_pieces[0], 'F');
        solution.insert(solution.end(), move.begin(), move.end());
        cube.apply_moves(move);

        int white_index = cube.find_white_corner_face_from_colours(colour1, colour2);
        for (const auto& [indices, moves] : corner_to_front_bottom_white_down) {
            if (std::find(indices.begin(), indices.end(), white_index) != indices.end()) {
                solution.insert(solution.end(), moves.begin(), moves.end());
                cube.apply_moves(moves);
            }
        }
        
        while (cube.check_corner_correct(colour1, colour2) == false) {
            cube.apply_moves(four_move_algo);
            solution.insert(solution.end(), four_move_algo.begin(), four_move_algo.end());
        }
        
    }

    return solution;
}