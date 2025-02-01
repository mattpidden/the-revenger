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

    std::vector<std::string> reduce_centres_solution = reduce_centres(cube);
    solution.insert(solution.end(), reduce_centres_solution.begin(), reduce_centres_solution.end());

    std::vector<std::string> reduce_edges_solution = reduce_edges(cube);
    solution.insert(solution.end(), reduce_edges_solution.begin(), reduce_edges_solution.end());

    std::vector<int> white_centre_pieces = cube.find_centre_pieces('W');
    std::vector<std::string> move = cube.rotate_cube_so_piece_on_face(white_centre_pieces[0], 'U');
    solution.insert(solution.end(), move.begin(), move.end());
    cube.apply_moves(move);



    cube.display_cube();    
    std::cout << "Total Moves: " << solution.size() << "\n";
    cube.verify_valid_cube();

    return 0;
}
