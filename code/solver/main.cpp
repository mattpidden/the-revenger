#include "vector_cube.h"
#include "reduce_centres.h"
#include <map>
#include <algorithm>
#include <utility>

int main() {
    RubiksCube4x4 cube;
    std::vector<std::string> solution = {};

    cube.verify_valid_cube();
    cube.apply_random_moves(false);
    cube.verify_valid_cube();

    std::vector<std::string> reduce_centres_solution = reduce_centres(cube);
    solution.insert(solution.end(), reduce_centres_solution.begin(), reduce_centres_solution.end());

    std::vector<std::vector<std::vector<std::vector<int>>>> edges = cube.find_paired_edges();

    std::map<std::vector<int>, std::vector<std::string>> edge_piecie_moves_to_front_rt_edge = {
      {{1,34}, {"U", "R'"}}, {{2,33}, {"U2", "F"}}, 
      {{4,65}, {"U'", "F"}}, {{8,66}, {"U2", "R'"}}, 
      {{7,82}, {"R'"}}, {{11,81}, {"U", "F"}}, 
      {{13,49}, {"F'"}}, {{14,50}, {"U'", "R'"}}, 
      {{52,71}, {"F", "U'", "R'"}}, {{56,75}, {"F2"}},
      {{55,84}, {}}, {{59,88}, {"F'", "U'", "R'"}}, 
      {{61,17}, {"D", "R"}}, {{62,18}, {"F'"}}, 
      {{36,87}, {"R'", "U", "F"}}, {{40,91}, {"R2"}}, 
      {{39,68}, {"L2", "F2"}}, {{43,72}, {"L", "U'", "F"}}, 
      {{20,78}, {"D", "F'"}}, {{24,77}, {"L'", "F2"}},
      {{23,93}, {"R"}}, {{27,94}, {"D'", "F'"}}, 
      {{29,46}, {"D2", "F'"}}, {{30,45}, {"D'", "R"}}};
    std::map<std::vector<int>, std::vector<std::string>> edge_piecie_moves_to_front_lb_edge = {
      {{1,34}, {"U'", "L"}}, {{2,33}, {"B", "L2"}}, 
      {{4,65}, {"U", "B", "L2"}}, {{8,66}, {"L"}}, 
      {{7,82}, {"U2", "L"}}, {{11,81}, {"U'", "B", "L2"}}, 
      {{13,49}, {"U2", "B", "L2"}}, {{14,50}, {"U", "L"}}, 
      {{52,71}, {"L'", "U", "B", "L2"}}, {{56,75}, {}},
      {{55,84}, {}}, {{59,88}, {}}, 
      {{61,17}, {"D'", "L'"}}, {{62,18}, {"D2", "B'", "L2"}}, 
      {{36,87}, {"B'", "D", "L'"}}, {{40,91}, {"B2", "L2"}}, 
      {{39,68}, {"L2"}}, {{43,72}, {"L", "U", "B", "L2"}}, 
      {{20,78}, {"D'", "B'", "L2"}}, {{24,77}, {"L'"}},
      {{23,93}, {"D2", "L'"}}, {{27,94}, {"D", "B'", "L2"}}, 
      {{29,46}, {"B'", "L2"}}, {{30,45}, {"D", "L'"}},};

    while (edges[1].size() > 2) {
        std::vector<int> edge1 = edges[1][0][0];
        std::vector<std::string> right_edge_moves = edge_piecie_moves_to_front_rt_edge[edge1];
        solution.insert(solution.end(), right_edge_moves.begin(), right_edge_moves.end());
        cube.apply_moves(right_edge_moves);

        std::vector<int> edge2 = cube.find_matching_edge_piece({55,84});
        std::vector<std::string> left_edge_moves = edge_piecie_moves_to_front_lb_edge[edge2];
        solution.insert(solution.end(), left_edge_moves.begin(), left_edge_moves.end());
        cube.apply_moves(left_edge_moves);

        std::vector<std::string> edge_pair_algo = {"Dw", "R", "U", "R'", "Dw'"};
        solution.insert(solution.end(), edge_pair_algo.begin(), edge_pair_algo.end());
        cube.apply_moves(edge_pair_algo);
        
        edges = cube.find_paired_edges();
        if (edges[1].size() > 0) {
            std::vector<std::string> rotation_moves = cube.rotate_cube_so_piece_on_face(edges[1][1][0][0], 'U');
            solution.insert(solution.end(), rotation_moves.begin(), rotation_moves.end());
            cube.apply_moves(rotation_moves);
            edges = cube.find_paired_edges();
        }
        
    }

    std::map<std::vector<int>, std::vector<std::string>> edge_bar_moves_to_front_right = {
      {{1,2,33,34}, {"U", "R'"}},
      {{4,8,65,66}, {"U'", "F"}},
      {{7,11,81,82}, {"R'"}},
      {{13,14,49,50}, {"F"}}, 
      {{52,56,71,75}, {"F2"}},
      {{55,59,84,88}, {}}, 
      {{61,62,17,18}, {"F'"}},
      {{36,40,87,91}, {"R2"}}, 
      {{39,43,68,72}, {"L2", "F2"}}, 
      {{20,24,77,78}, {"D", "F'"}}, 
      {{23,27,93,94}, {"R"}}, 
      {{29,30,45,46}, {"D'", "R"}}};

    std::map<std::vector<int>, std::vector<std::string>> edge_bar_moves_to_front_left = {
      {{1,2,33,34}, {"U'", "L"}},
      {{4,8,65,66}, {"L"}},
      {{7,11,81,82}, {"U2", "L"}},
      {{13,14,49,50}, {"U", "L"}}, 
      {{52,56,71,75}, {}},
      {{55,59,84,88}, {}}, 
      {{61,62,17,18}, {"D'", "L'"}},
      {{36,40,87,91}, {"B2", "L2"}}, 
      {{39,43,68,72}, {"L2"}}, 
      {{20,24,77,78}, {"L'"}}, 
      {{23,27,93,94}, {"D2", "L'"}}, 
      {{29,30,45,46}, {"D", "L2"}}};

    if (!edges[1].empty()) {
        int target_value = edges[1][0][0][0];
        std::vector<std::string> rotation_moves;

        for (const auto& pair : edge_bar_moves_to_front_right) {
            if (std::find(pair.first.begin(), pair.first.end(), target_value) != pair.first.end()) {
                rotation_moves = pair.second;
                break;
            }
        }

        if (!rotation_moves.empty()) {
            solution.insert(solution.end(), rotation_moves.begin(), rotation_moves.end());
            cube.apply_moves(rotation_moves);
        }

        target_value = edges[1][1][0][0];
        for (const auto& pair : edge_bar_moves_to_front_left) {
            if (std::find(pair.first.begin(), pair.first.end(), target_value) != pair.first.end()) {
                rotation_moves = pair.second;
                break;
            }
        }

        if (!rotation_moves.empty()) {
            solution.insert(solution.end(), rotation_moves.begin(), rotation_moves.end());
            cube.apply_moves(rotation_moves);
        }

        std::vector<std::string> edge_flip = cube.decide_edge_flip();
        solution.insert(solution.end(), edge_flip.begin(), edge_flip.end());
        cube.apply_moves(edge_flip);

        std::vector<std::string> final_edge_algo = {"Dw", "R", "F'", "U", "R'", "F", "Dw'"};
        solution.insert(solution.end(), final_edge_algo.begin(), final_edge_algo.end());
        cube.apply_moves(final_edge_algo);
    }

    

    cube.display_cube();    
    std::cout << "Total Moves: " << solution.size() << "\n";
    cube.verify_valid_cube();

    return 0;
}
