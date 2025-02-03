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

#include "middle_layer.h"
#include "vector_cube.h"

std::vector<std::string> solve_middle_layer(RubiksCube4x4& cube) { 
    std::vector<std::string> solution = {};
    
    // Solve middle layer
    std::map<std::vector<int>, std::vector<std::string>> insert_edges = {
        {{1,2},   {"U", "y", "U'", "L'", "U", "L", "U", "F", "U'", "F'", "y'"}},
        {{33,34}, {"U2", "U", "R", "U'", "R'", "U'", "F'", "U", "F"}},
        {{4,8},   {"U2", "y", "U'", "L'", "U", "L", "U", "F", "U'", "F'", "y'"}},
        {{65,66}, {"U'", "U", "R", "U'", "R'", "U'", "F'", "U", "F"}},
        {{7,11},  {"y", "U'", "L'", "U", "L", "U", "F", "U'", "F'", "y'"}},
        {{81,82}, {"U", "U", "R", "U'", "R'", "U'", "F'", "U", "F"}},
        {{13,14}, {"U'", "y", "U'", "L'", "U", "L", "U", "F", "U'", "F'", "y'"}},
        {{49,50}, {"U", "R", "U'", "R'", "U'", "F'", "U", "F"}},
        {{55,59}, {}},
        {{52,56}, {"U'", "L'", "U", "L", "U", "F", "U'", "F'", "U", "y", "U'", "L'", "U", "L", "U", "F", "U'", "F'", "y'"}},
        {{71,75}, {"U'", "L'", "U", "L", "U", "F", "U'", "F'", "U2", "U", "R", "U'", "R'", "U'", "F'", "U", "F"}},
        {{84,88}, {"U", "R", "U'", "R'", "U'", "F'", "U", "F", "U2", "U", "R", "U'", "R'", "U'", "F'", "U", "F"}},
        {{36,40}, {"y", "R", "U'", "R'", "U'", "F'", "U", "F", "y'", "R", "U'", "R'", "U'", "F'", "U", "F"}},
        {{87,91}, {"y", "U", "R", "U'", "R'", "U'", "F'", "U", "F", "y'", "U2", "y", "U'", "L'", "U", "L", "U", "F", "U'", "F'", "y'"}},
        {{39,43}, {"y2", "R", "U'", "R'", "U'", "F'", "U", "F", "y2", "U'", "y", "U'", "L'", "U", "L", "U", "F", "U'", "F'", "y'"}},
        {{68,72}, {"y'", "L'", "U", "L", "U", "F", "U'", "F'", "y2", "U'", "L'", "U", "L", "U", "F", "U'", "F'", "y'"}}};

    std::vector<char> colours = {'G', 'O', 'B', 'R'};

    // loop over each colour, 
    for (size_t i = 0; i < colours.size(); i++) {
        char colour1 = colours[i];
        char colour2 = colours[(i + 1) % colours.size()];

        // orientate that face to front
        std::vector<int> colour_centre_pieces = cube.find_centre_pieces(colour1);
        std::vector<std::string> move = cube.rotate_cube_so_piece_on_face(colour_centre_pieces[0], 'F');
        solution.insert(solution.end(), move.begin(), move.end());
        cube.apply_moves(move);

        // find the edge piece that it that colour and the next COLOUR
        std::vector<int> colour1_edge_location = cube.find_edge_by_colour(colour1, colour2);
        std::cout << "Inserting : " << colour1 << " and " << colour2 << " edge from location " << colour1_edge_location[0] << ", " << colour1_edge_location[1] << "\n";

        // apply algo to place it on front top middle
        std::vector<std::string> moves_to_insert_edge = insert_edges[colour1_edge_location];
        solution.insert(solution.end(), moves_to_insert_edge.begin(), moves_to_insert_edge.end());
        cube.apply_moves(moves_to_insert_edge);
    }

    return solution;
}