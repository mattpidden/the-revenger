#include <map>
#include <algorithm>
#include <utility>

#include "vector_cube.h"
#include "reduce_centres.h"
#include "reduce_edges.h"
#include "white_cross.h"
#include "first_layer.h"

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
    std::map<std::vector<int>, std::vector<std::string>> insert_edges = {
        {{1,2},   {"U", "y", "U'", "L'", "U", "L", "U", "F", "U'", "F'", "y'"}}, //good
        {{33,34}, {"U2", "U", "R", "U'", "R'", "U'", "F'", "U", "F"}}, 
        {{4,8},   {"U2", "y", "U'", "L'", "U", "L", "U", "F", "U'", "F'", "y'"}}, //good
        {{65,66}, {"U'", "U", "R", "U'", "R'", "U'", "F'", "U", "F"}}, 
        {{7,11},  {"y", "U'", "L'", "U", "L", "U", "F", "U'", "F'", "y'"}}, // good
        {{81,82}, {"U", "U", "R", "U'", "R'", "U'", "F'", "U", "F"}}, 
        {{13,14}, {"U'", "y", "U'", "L'", "U", "L", "U", "F", "U'", "F'", "y'"}}, //good
        {{49,50}, {"U", "R", "U'", "R'", "U'", "F'", "U", "F"}}, 
        {{55,59}, {}},
        {{52,56}, {"U'", "L'", "U", "L", "U", "F", "U'", "F'", "U", "y", "U'", "L'", "U", "L", "U", "F", "U'", "F'", "y'"}},
        {{71,75}, {"U'", "L'", "U", "L", "U", "F", "U'", "F'", "U2", "U", "R", "U'", "R'", "U'", "F'", "U", "F"}}, 
        {{84,88}, {"U", "R", "U'", "R'", "U'", "F'", "U", "F", "U2", "U", "R", "U'", "R'", "U'", "F'", "U", "F"}},
        {{36,40}, {"y", "R", "U'", "R'", "U'", "F'", "U", "F", "y'", "R", "U'", "R'", "U'", "F'", "U", "F"}},
        {{87,91}, {"y", "U", "R", "U'", "R'", "U'", "F'", "U", "F", "y'", "U2", "y", "U'", "L'", "U", "L", "U", "F", "U'", "F'", "y'"}},
        {{39,43}, {"y2", "R", "U'", "R'", "U'", "F'", "U", "F", "y2", "U'", "y", "U'", "L'", "U", "L", "U", "F", "U'", "F'", "y'"}},
        {{68,72}, {"y'", "L'", "U", "L", "U", "F", "U'", "F'", "y2", "U'", "L'", "U", "L", "U", "F", "U'", "F'", "y'"}}};
    //81,82
    //68,72
    //7,11
    //52,56

    //13,14
    //68,72
    //81,82,
    //65,66
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


    cube.display_cube();    
    std::cout << "Total Moves: " << solution.size() << "\n";
    cube.verify_valid_cube();

    return 0;
}
