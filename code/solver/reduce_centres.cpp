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

#include "reduce_centres.h"
#include "vector_cube.h"

std::vector<std::string> reduce_centres(RubiksCube4x4& cube) {
    std::vector<std::string> solution = {};
    std::vector<std::string> white_centre_solution = create_white_centre(cube);
    solution.insert(solution.end(), white_centre_solution.begin(), white_centre_solution.end());
    std::cout << "Created white centre." << "\n";
    solution.push_back("x2");
    cube.apply_move("x2");
    std::vector<std::string> yellow_centre_solution = create_any_centre(cube, 'Y');
    solution.insert(solution.end(), yellow_centre_solution.begin(), yellow_centre_solution.end());
    std::cout << "Created yellow centre." << "\n";    
    solution.push_back("z");
    cube.apply_move("z");
    std::vector<std::string> orange_centre_solution = create_any_centre(cube, 'O');
    solution.insert(solution.end(), orange_centre_solution.begin(), orange_centre_solution.end());
    std::cout << "Created orange centre." << "\n";    
    solution.push_back("x");
    cube.apply_move("x");
    std::vector<std::string> blue_centre_solution = create_any_centre(cube, 'B');
    solution.insert(solution.end(), blue_centre_solution.begin(), blue_centre_solution.end());
    std::cout << "Created blue centre." << "\n";    
    solution.push_back("x");
    cube.apply_move("x");
    std::vector<std::string> red_centre_solution = create_any_centre(cube, 'R');
    solution.insert(solution.end(), red_centre_solution.begin(), red_centre_solution.end());
    std::cout << "Created red centre." << "\n";    
    solution.push_back("x");
    cube.apply_move("x");
    std::vector<std::string> green_centre_solution = create_any_centre(cube, 'G');
    solution.insert(solution.end(), green_centre_solution.begin(), green_centre_solution.end());
    std::cout << "Created green centre." << "\n";    
    solution.push_back("z");
    cube.apply_move("z");
    solution.push_back("y");
    cube.apply_move("y");
    return solution;
}



std::vector<std::string> create_white_centre(RubiksCube4x4& cube) {
    std::map<int, std::vector<std::string>> centre_piecie_moves_to_front_tr_centre = {{53, {"F"}}, {54, {}}, {57, {"F2"}}, {58, {"F'"}}, {85, {"u", "F"}}, {86, {"u"}}, {89, {"d'", "F2"}}, {90, {"d'", "F'"}}, {69, {"u'", "F"}}, {70, {"u'"}}, {73, {"d", "F2"}}, {74, {"d", "F'"}}, {37, {"u2", "F"}}, {38, {"u2"}}, {41, {"d2", "F2"}}, {42, {"d2", "F'"}}, {21, {"l'", "F", "l"}}, {22, {"r", "F'", "r'", "F"}}, {25, {"l'", "F2", "l"}}, {26, {"r", "F", "r'", "F2"}},};

    std::vector<std::string> solution = {};
    std::vector<int> white_centre_pieces = cube.find_centre_pieces('W');
    auto [white_centre_pieces_on_U, non_white_centre_pieces_on_U]  = cube.find_spots_in_centre('U', 'W');
    
    if (white_centre_pieces_on_U.size() == 4) {
      return solution;
    }
    
    if (white_centre_pieces_on_U.size() == 0) {
      std::vector<std::string> move = cube.rotate_cube_so_piece_on_face(white_centre_pieces[0], 'U');
      solution.insert(solution.end(), move.begin(), move.end());
      cube.apply_moves(move);
    }
    std::tie(white_centre_pieces_on_U, non_white_centre_pieces_on_U) = cube.find_spots_in_centre('U', 'W');
    
    // Now there is at least 1 white centre piece on U
    while (white_centre_pieces_on_U.size() != 4) {
        while (std::find(non_white_centre_pieces_on_U.begin(), non_white_centre_pieces_on_U.end(), 6) == non_white_centre_pieces_on_U.end()) {
          solution.push_back("U");
          cube.apply_move("U");
          std::tie(white_centre_pieces_on_U, non_white_centre_pieces_on_U) = cube.find_spots_in_centre('U', 'W');
        }
        
        std::vector<int> white_centres_not_on_U;
        white_centre_pieces = cube.find_centre_pieces('W');
        std::sort(white_centre_pieces.begin(), white_centre_pieces.end());
        std::sort(white_centre_pieces_on_U.begin(), white_centre_pieces_on_U.end());
        std::set_difference(white_centre_pieces.begin(), white_centre_pieces.end(), white_centre_pieces_on_U.begin(), white_centre_pieces_on_U.end(), std::back_inserter(white_centres_not_on_U));
        
        std::vector<std::string> moves = centre_piecie_moves_to_front_tr_centre[white_centres_not_on_U[0]];
        std::vector<std::string> centre_insert_algo = {"r", "U'", "l'", "U", "r'", "U'", "l", "U"};
        solution.insert(solution.end(), moves.begin(), moves.end());
        cube.apply_moves(moves);
        solution.insert(solution.end(), centre_insert_algo.begin(), centre_insert_algo.end());
        cube.apply_moves(centre_insert_algo);
        
        std::tie(white_centre_pieces_on_U, non_white_centre_pieces_on_U) = cube.find_spots_in_centre('U', 'W');
    }

    return solution;
}

std::vector<std::string> create_any_centre(RubiksCube4x4& cube, char colour) {
    std::map<int, std::vector<std::string>> centre_piecie_moves_to_front_tr_centre = {{53, {"F"}}, {54, {}}, {57, {"F2"}}, {58, {"F'"}}, {85, {"u", "F"}}, {86, {"u"}}, {89, {"d'", "F2"}}, {90, {"d'", "F'"}}, {69, {"u'", "F"}}, {70, {"u'"}}, {73, {"d", "F2"}}, {74, {"d", "F'"}}, {37, {"u2", "F"}}, {38, {"u2"}}, {41, {"d2", "F2"}}, {42, {"d2", "F'"}}, {21, {"l'", "F", "l"}}, {22, {"r", "F'", "r'", "F"}}, {25, {"l'", "F2", "l"}}, {26, {"r", "F", "r'", "F2"}},};
    std::vector<std::string> solution = {};



    std::vector<int> yellow_centre_pieces = cube.find_centre_pieces(colour);
    auto [yellow_centre_pieces_on_U, non_yellow_centre_pieces_on_U]  = cube.find_spots_in_centre('U', colour);
    
    if (yellow_centre_pieces_on_U.size() == 4) {
      return solution;
    }
    
    while (yellow_centre_pieces_on_U.size() != 4) {
        while (std::find(non_yellow_centre_pieces_on_U.begin(), non_yellow_centre_pieces_on_U.end(), 6) == non_yellow_centre_pieces_on_U.end()) {
          solution.push_back("U");
          cube.apply_move("U");
          std::tie(yellow_centre_pieces_on_U, non_yellow_centre_pieces_on_U) = cube.find_spots_in_centre('U', colour);
        }
        
        std::vector<int> yellow_centres_not_on_U;
        yellow_centre_pieces = cube.find_centre_pieces(colour);
        std::sort(yellow_centre_pieces.begin(), yellow_centre_pieces.end());
        std::sort(yellow_centre_pieces_on_U.begin(), yellow_centre_pieces_on_U.end());
        std::set_difference(yellow_centre_pieces.begin(), yellow_centre_pieces.end(), yellow_centre_pieces_on_U.begin(), yellow_centre_pieces_on_U.end(), std::back_inserter(yellow_centres_not_on_U));
        
        std::vector<std::string> moves = centre_piecie_moves_to_front_tr_centre[yellow_centres_not_on_U[0]];
        std::vector<std::string> centre_insert_algo = {"r", "U'", "l'", "U", "r'", "U'", "l", "U"};
        solution.insert(solution.end(), moves.begin(), moves.end());
        cube.apply_moves(moves);
        solution.insert(solution.end(), centre_insert_algo.begin(), centre_insert_algo.end());
        cube.apply_moves(centre_insert_algo);
        std::vector<std::string> opposite_moves = cube.apply_opposite_moves(moves);
        solution.insert(solution.end(), opposite_moves.begin(), opposite_moves.end());
        
        std::tie(yellow_centre_pieces_on_U, non_yellow_centre_pieces_on_U) = cube.find_spots_in_centre('U', colour);
    }

    return solution;
}



