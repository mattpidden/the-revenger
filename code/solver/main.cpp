#include "vector_cube.h"
#include "reduce_centres.h"

int main() {
    RubiksCube4x4 cube;
    std::vector<std::string> solution = {};

    cube.verify_valid_cube();
    cube.apply_random_moves(false);
    cube.verify_valid_cube();

    std::vector<std::string> white_centre_solution = create_white_centre(cube);
    solution.insert(solution.end(), white_centre_solution.begin(), white_centre_solution.end());
    solution.push_back("x2");
    cube.apply_move("x2");
    std::vector<std::string> yellow_centre_solution = create_any_centre(cube, 'Y');
    solution.insert(solution.end(), yellow_centre_solution.begin(), yellow_centre_solution.end());
    solution.push_back("z");
    cube.apply_move("z");
    std::vector<std::string> orange_centre_solution = create_any_centre(cube, 'O');
    solution.insert(solution.end(), orange_centre_solution.begin(), orange_centre_solution.end());
    solution.push_back("x");
    cube.apply_move("x");
    std::vector<std::string> blue_centre_solution = create_any_centre(cube, 'B');
    solution.insert(solution.end(), blue_centre_solution.begin(), blue_centre_solution.end());
    solution.push_back("x");
    cube.apply_move("x");
    std::vector<std::string> red_centre_solution = create_any_centre(cube, 'R');
    solution.insert(solution.end(), red_centre_solution.begin(), red_centre_solution.end());
    solution.push_back("x");
    cube.apply_move("x");
    std::vector<std::string> green_centre_solution = create_any_centre(cube, 'G');
    solution.insert(solution.end(), green_centre_solution.begin(), green_centre_solution.end());
    solution.push_back("z");
    cube.apply_move("z");
    solution.push_back("y");
    cube.apply_move("y");

    cube.display_cube();    
    std::cout << "Total Moves: " << solution.size() << "\n";
    cube.verify_valid_cube();

    return 0;
}
