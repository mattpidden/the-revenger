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

#include "white_cross.h"
#include "vector_cube.h"

std::vector<std::string> solve_white_cross(RubiksCube4x4& cube) {
    std::vector<std::string> solution = {};

    // Rotate cube so white centre on the bottom and green on front
    std::vector<int> white_centre_pieces = cube.find_centre_pieces('W');
    std::vector<std::string> move = cube.rotate_cube_so_piece_on_face(white_centre_pieces[0], 'D');
    solution.insert(solution.end(), move.begin(), move.end());
    cube.apply_moves(move);


    std::map<std::vector<int>, std::vector<std::string>> edge_bar_moves_to_front_cross = {
        {{1,2},   {"U2", "F2"}}, 
        {{33,34}, {"U", "R'", "F", "R"}}, 
        {{4,8},   {"U'", "F2"}}, 
        {{65,66}, {"L", "F'", "L'"}}, 
        {{7,11},  {"U", "F2"}}, 
        {{81,82}, {"R'", "F", "R"}}, 
        {{13,14}, {"F2"}}, 
        {{49,50}, {"U", "L", "F'", "L'"}}, 
        {{52,56}, {"L'", "U'", "L", "F2"}}, 
        {{71,75}, {"F'"}}, 
        {{55,59}, {"R", "U", "R'", "F2"}}, 
        {{84,88}, {"F"}}, 
        {{61,62}, {"F", "L'", "U'", "L", "F2"}}, 
        {{17,18}, {}}, 
        {{36,40}, {"R'", "U", "R", "F2"}}, 
        {{87,91}, {"R2", "F", "R2"}}, 
        {{39,43}, {"L", "U'", "L'", "F2"}}, 
        {{68,72}, {"L2", "F'", "L2"}}, 
        {{20,24}, {"L2", "U'", "F2"}}, 
        {{77,78}, {"L'", "F'"}}, 
        {{23,27}, {"R2", "U", "F2"}}, 
        {{93,94}, {"R", "F"}}, 
        {{29,30}, {"B2", "U2", "F2"}},
        {{45,46}, {"B", "R'", "U", "R", "F2"}}};

    std::vector<char> colours = {'G', 'O', 'B', 'R'};

    for (const auto& colour : colours) {
        std::vector<int> colour_centre_pieces = cube.find_centre_pieces(colour);
        move = cube.rotate_cube_so_piece_on_face(colour_centre_pieces[0], 'F');
        solution.insert(solution.end(), move.begin(), move.end());
        cube.apply_moves(move);

        std::vector<int> white_edge_location = cube.find_white_edge_bar_for_colour(colour);
        std::vector<std::string> moves_to_cross = edge_bar_moves_to_front_cross[white_edge_location];
        solution.insert(solution.end(), moves_to_cross.begin(), moves_to_cross.end());
        cube.apply_moves(moves_to_cross);

    }

    return solution;
}
