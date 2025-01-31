#include "vector_cube.h"
#include "reduce_centres.h"

int main() {
    RubiksCube4x4 cube;
    std::vector<std::string> solution = {};

    cube.verify_valid_cube();
    cube.apply_random_moves(false);
    cube.verify_valid_cube();

    std::vector<std::string> reduce_centres_solution = reduce_centres(cube);
    solution.insert(solution.end(), reduce_centres_solution.begin(), reduce_centres_solution.end());

    std::vector<std::vector<std::vector<std::vector<int>>>> edges = cube.find_unpaired_edges();
    if (edges[0].size() > 0) {
      std::string move = cube.rotate_cube_so_piece_on_face(edges[0][0][0][0], 'U');
      solution.push_back(move);
      cube.apply_move(move);
    }



    cube.display_cube();    
    std::cout << "Total Moves: " << solution.size() << "\n";
    cube.verify_valid_cube();

    return 0;
}
