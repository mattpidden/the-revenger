#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <sstream>
#include <random>
#include <unordered_set>
#include <utility>

#include "six_4x4_cube.h"

RubiksCube4x4::RubiksCube4x4() {
    char colors[6] = {'W', 'Y', 'B', 'G', 'O', 'R'};
    for (int f = 0; f < 6; ++f) {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                cube[f][i][j] = colors[f];
            }
        }
    }
}

void RubiksCube4x4::display_cube() {
    std::map<char, std::string> colours_to_emojis = {
        {'W', "⬜"}, {'Y', "🟨"}, {'G', "🟩"}, 
        {'B', "🟦"}, {'O', "🟧"}, {'R', "🟥"}
    };

    // Print the Up (U) face
    std::cout << "          Up (0)\n";
    for (int i = 0; i < 4; ++i) {
        std::cout << "          ";
        for (int j = 0; j < 4; ++j) {
            std::cout << colours_to_emojis[cube[0][i][j]];  // U face
        }
        std::cout << "\n";
    }

    // Print the Left (L), Front (F), Right (R), and Back (B) faces
    std::cout << "\nLeft (4)  Front (3) Right (5) Back (2)\n";
    for (int row = 0; row < 4; ++row) {
        // Left (L) face
        for (int col = 0; col < 4; ++col)
            std::cout << colours_to_emojis[cube[4][row][col]];
        std::cout << "  ";
        
        // Front (F) face
        for (int col = 0; col < 4; ++col)
            std::cout << colours_to_emojis[cube[3][row][col]];
        std::cout << "  ";
        
        // Right (R) face
        for (int col = 0; col < 4; ++col)
            std::cout << colours_to_emojis[cube[5][row][col]];
        std::cout << "  ";
        
        // Back (B) face
        for (int col = 0; col < 4; ++col)
            std::cout << colours_to_emojis[cube[2][row][col]];
        std::cout << "\n";
    }

    // Print the Down (D) face
    std::cout << "\n          Down (1)\n";
    for (int i = 0; i < 4; ++i) {
        std::cout << "          ";
        for (int j = 0; j < 4; ++j) {
            std::cout << colours_to_emojis[cube[1][i][j]];  // D face
        }
        std::cout << "\n";
    }

    std::cout << "\n";
}




std::vector<std::string> RubiksCube4x4::generate_random_moves(int num) {
    std::vector<std::string> moves = {"U", "U2", "U'", "D", "D2", "D'", "F", "F2", "F'", "B", "B2", "B'", "R", "R2", "R'", "L", "L2", "L'"};
    std::random_device rd;
    std::mt19937 rng(rd());

    std::vector<std::string> scramble;

    for (int i = 0; i < num; i++) {
        std::uniform_int_distribution<int> dist(0, moves.size() - 1);
        scramble.push_back(moves[dist(rng)]);
    }

    return scramble;
}

void RubiksCube4x4::apply_moves(const std::vector<std::string> moves) {
    for (const std::string& move : moves) {
        apply_move(move);
    }
}

void RubiksCube4x4::apply_move(const std::string move) {
    bool clockwise = (move.back() != '\'');
    std::string base_move = move;
    
    if (!clockwise) base_move.pop_back();
    int repeat = (base_move.back() == '2') ? 2 : 1;
    if (repeat == 2) base_move.pop_back();

    for (int i = 0; i < repeat; i++) {
        if (base_move == "R") move_R();
        else if (base_move == "L") move_L();
        else if (base_move == "U") move_U();
        else if (base_move == "D") move_D();
        else if (base_move == "F") move_F();
        else if (base_move == "B") move_B();
        else std::cout << "Invalid move: " << move << "\n";
    }
}

void RubiksCube4x4::move_R() {
    // Rotate the top face itself
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 3; j++) {
            cube[0][i][j] = cube[0][2-j][i];  // Transpose and reverse for 90 deg rotation
        }
    }

    // Adjust adjacent face pieces
    // Left -> Front -> Right -> Back
    for (int i = 0; i < 4; i++) {
        char tempLeft = cube[5][0][i];
        cube[4][0][i] = cube[3][0][i];
        cube[2][0][i] = cube[4][0][i];
        cube[5][0][i] = cube[2][0][i];
        cube[3][0][i] = tempLeft;
    }
}

void RubiksCube4x4::move_L() {

    // Rotate the top face itself
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 3; j++) {
            cube[0][i][j] = cube[0][2-j][i];  // Transpose and reverse for 90 deg rotation
        }
    }

    // Adjust adjacent face pieces
    // Left -> Front -> Right -> Back
    for (int i = 0; i < 4; i++) {
        char tempLeft = cube[5][0][i];
        cube[4][0][i] = cube[3][0][i];
        cube[2][0][i] = cube[4][0][i];
        cube[5][0][i] = cube[2][0][i];
        cube[3][0][i] = tempLeft;
    }
}

void RubiksCube4x4::move_U() {
// Rotate the top face itself
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 3; j++) {
            cube[0][i][j] = cube[0][2-j][i];  // Transpose and reverse for 90 deg rotation
        }
    }

    // Adjust adjacent face pieces
    // Left -> Front -> Right -> Back
    for (int i = 0; i < 4; i++) {
        char tempLeft = cube[5][0][i];
        cube[4][0][i] = cube[3][0][i];
        cube[2][0][i] = cube[4][0][i];
        cube[5][0][i] = cube[2][0][i];
        cube[3][0][i] = tempLeft;
    }
}

void RubiksCube4x4::move_D() {
// Rotate the top face itself
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 3; j++) {
            cube[0][i][j] = cube[0][2-j][i];  // Transpose and reverse for 90 deg rotation
        }
    }

    // Adjust adjacent face pieces
    // Left -> Front -> Right -> Back
    for (int i = 0; i < 4; i++) {
        char tempLeft = cube[5][0][i];
        cube[4][0][i] = cube[3][0][i];
        cube[2][0][i] = cube[4][0][i];
        cube[5][0][i] = cube[2][0][i];
        cube[3][0][i] = tempLeft;
    }
}

void RubiksCube4x4::move_F() {

// Rotate the top face itself
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 3; j++) {
            cube[0][i][j] = cube[0][2-j][i];  // Transpose and reverse for 90 deg rotation
        }
    }

    // Adjust adjacent face pieces
    // Left -> Front -> Right -> Back
    for (int i = 0; i < 4; i++) {
        char tempLeft = cube[5][0][i];
        cube[4][0][i] = cube[3][0][i];
        cube[2][0][i] = cube[4][0][i];
        cube[5][0][i] = cube[2][0][i];
        cube[3][0][i] = tempLeft;
    }
}

void RubiksCube4x4::move_B() {

// Rotate the top face itself
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 3; j++) {
            cube[0][i][j] = cube[0][2-j][i];  // Transpose and reverse for 90 deg rotation
        }
    }

    // Adjust adjacent face pieces
    // Left -> Front -> Right -> Back
    for (int i = 0; i < 4; i++) {
        char tempLeft = cube[5][0][i];
        cube[4][0][i] = cube[3][0][i];
        cube[2][0][i] = cube[4][0][i];
        cube[5][0][i] = cube[2][0][i];
        cube[3][0][i] = tempLeft;
    }
}
