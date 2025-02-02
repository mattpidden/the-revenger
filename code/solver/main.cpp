#include <map>
#include <algorithm>
#include <utility>

#include "vector_cube.h"
#include "reduce_centres.h"
#include "reduce_edges.h"

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

    // Rotate cube so white centre on the bottom and green on front
    std::vector<int> white_centre_pieces = cube.find_centre_pieces('W');
    std::vector<std::string> move = cube.rotate_cube_so_piece_on_face(white_centre_pieces[0], 'D');
    solution.insert(solution.end(), move.begin(), move.end());
    cube.apply_moves(move);
    std::vector<int> green_centre_pieces = cube.find_centre_pieces('G');
    move = cube.rotate_cube_so_piece_on_face(green_centre_pieces[0], 'F');
    solution.insert(solution.end(), move.begin(), move.end());
    cube.apply_moves(move);

    // Create white cross on the bottom
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
        {{45,46}, {"B", "R2", "F", "R2"}}};
    

    cube.display_cube();    
    std::cout << "Total Moves: " << solution.size() << "\n";
    cube.verify_valid_cube();

    return 0;
}
