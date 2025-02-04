#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <sstream> 
#include <random>
#include <unordered_set>
#include <utility>

#include "vector_cube.h"

RubiksCube4x4::RubiksCube4x4() {
    facelets = {
        'W', 'W', 'W', 'W', 'Y', 'Y', 'Y', 'Y', 'W', 'W', 'W', 'W', 'Y', 'Y', 'Y', 'Y',  // Up face  
        'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y',  // Down face
        'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B',  // Back face
        'G', 'G', 'G', 'G', 'G', 'G', 'G', 'G', 'G', 'G', 'G', 'G', 'G', 'G', 'G', 'G',  // Front face
        'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O',  // Left face
        'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R',  // Right face
    };
}

/*
             Up (U)
             0  1  2  3
             4  5  6  7
             8  9  10 11
             12 13 14 15

Left (L)     Front (F)    Right (R)    Back (B)
64 65 66 67  48 49 50 51  80 81 82 83  32 33 34 35
68 69 70 71  52 53 54 55  84 85 86 87  36 37 38 39
72 73 74 75  56 57 58 59  88 89 90 91  40 41 42 43
76 77 78 79  60 61 62 63  92 93 94 95  44 45 46 47

             Down (D)
             16 17 18 19
             20 21 22 23
             24 25 26 27
             28 29 30 31
*/

void RubiksCube4x4::display_cube() {
    std::map<char, std::string> colours_to_emojis = {
        {'W', "⬜"}, {'Y', "🟨"}, {'G', "🟩"}, 
        {'B', "🟦"}, {'O', "🟧"}, {'R', "🟥"}
    };

    std::cout << "          Up (U)\n";
    for (int i = 0; i < 4; ++i) {
        std::cout << "          ";
        for (int j = 0; j < 4; ++j) {
            std::cout << colours_to_emojis[facelets[0 + i * 4 + j]];
        }
        std::cout << "\n";
    }
    std::cout << "\nLeft (L)  Front (F) Right (R) Back (B)\n";
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col)
            std::cout << colours_to_emojis[facelets[64 + row * 4 + col]];
        std::cout << "  ";
        for (int col = 0; col < 4; ++col)
            std::cout << colours_to_emojis[facelets[48 + row * 4 + col]];
        std::cout << "  ";
        for (int col = 0; col < 4; ++col)
            std::cout << colours_to_emojis[facelets[80 + row * 4 + col]];
        std::cout << "  ";
        for (int col = 0; col < 4; ++col)
            std::cout << colours_to_emojis[facelets[32 + row * 4 + col]];
        std::cout << "\n";
    }
    std::cout << "\n          Down (D)\n";
    for (int i = 0; i < 4; ++i) {
        std::cout << "          ";
        for (int j = 0; j < 4; ++j) {
            std::cout << colours_to_emojis[facelets[16 + i * 4 + j]];
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

std::vector<std::string> RubiksCube4x4::generate_random_moves(int num) {
    std::vector<std::string> moves = {"U", "U2", "U'", "D", "D2", "D'", "F", "F2", "F'", "B", "B2", "B'", "R", "R2", "R'","L", "L2", "L'"};
    std::random_device rd;
    std::mt19937 rng(rd());

    std::vector<std::string> scramble;

    for (int i = 0; i < num; i++) {
        std::uniform_int_distribution<int> dist(0, moves.size() - 1);
        std::string move = moves[dist(rng)];

        scramble.push_back(move);
    }

    return scramble;

}



void RubiksCube4x4::apply_moves(const std::vector<std::string> moves) {
    std::string move;
    
    for (const std::string& move : moves) {
            apply_move(move);
    }

}

void RubiksCube4x4::apply_move(const std::string move) {
    bool clockwise = (move.back() != '\''); 

    std::string base_move = move;
    if (!clockwise) base_move = move.substr(0, move.size() - 1); // Remove ' if present
    int repeat = (base_move.back() == '2') ? 2 : 1;
    if (repeat == 2) base_move = base_move.substr(0, base_move.size() - 1); // Remove '2' if present

    for (int i = 0; i < repeat; i++) {
        if (base_move == "R") move_U(clockwise);
        else if (base_move == "L") move_U(clockwise);
        else if (base_move == "U") move_U(clockwise);
        else if (base_move == "D") move_U(clockwise);
        else if (base_move == "F") move_U(clockwise);
        else if (base_move == "B") move_U(clockwise);
        else std::cout << "Invalid move: " << move << "\n";
    }
}

void RubiksCube4x4::apply_index_swaps(const std::unordered_map<int, int> index_move_map, bool clockwise) {
    std::vector<char> facelets_copy = facelets;
    for (const auto& [a, b] : index_move_map) {
        if (clockwise) {
            facelets[a] = facelets_copy[b];
        } else {
            facelets[b] = facelets_copy[a];
        }
    }
}


void RubiksCube4x4::move_R(bool clockwise) {
    std::unordered_map<int, int> index_move_map = {
        {3, 51}, {7, 55}, {11, 59}, {15, 63}, {51, 19}, {55, 23}, {59, 27}, {63, 31}, {19, 44}, {23,40}, {27, 36}, {31, 32}, {44, 3}, {40, 7}, {36, 11}, {32,15},
        {80, 92}, {81, 88}, {82, 84}, {83, 80}, {84, 93}, {85, 89}, {86, 85}, {87, 81}, {88, 94}, {89, 90}, {90, 86}, {91, 82}, {92, 95}, {93, 91}, {94, 87}, {95, 83}
    };       
    apply_index_swaps(index_move_map, clockwise);
}

void RubiksCube4x4::move_L(bool clockwise) {
    std::unordered_map<int, int> index_move_map = {
        {48, 0}, {52, 4}, {56, 8}, {60, 12}, {16, 48}, {20, 52}, {24, 56}, {28, 60}, {47, 16}, {43, 20}, {39, 24}, {35, 28}, {0, 47}, {4, 43}, {8, 39}, {12, 35},
        {64, 76}, {65, 72}, {66, 68}, {67, 64}, {68, 77}, {69, 73}, {70, 69}, {71, 65}, {72, 78}, {73, 74}, {74, 70}, {75, 66}, {76, 79}, {77, 75}, {78, 71}, {79, 67}
    };       
    apply_index_swaps(index_move_map, clockwise);
}

// 0 = 0
// 1 = 16
// 2 = 32
// 3 = 48
// 4 = 64
// 5 = 80
/*
    facelets = {
      0  'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W',  // Up face  
     16  'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y',  // Down face
     32  'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B',  // Back face
     48  'G', 'G', 'G', 'G', 'G', 'G', 'G', 'G', 'G', 'G', 'G', 'G', 'G', 'G', 'G', 'G',  // Front face
     64  'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O',  // Left face
     80  'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R',  // Right face
    };
*/


void RubiksCube4x4::move_U(bool clockwise) {

    char temp[4];  // Temporary array to hold the first column for rotation

    // Save the top row (indices 0, 1, 2, 3)
    for (int i = 0; i < 4; i++) {
        temp[i] = facelets[i]; // Save the first row (0, 1, 2, 3) from the U face
    }

    // Perform the 90 degree clockwise rotation on the U face
    for (int i = 0; i < 4; i++) {
        facelets[i] = facelets[12 + i * 4]; // First column of U face becomes the top row
        facelets[12 + i * 4] = facelets[15 - i]; // Last column becomes the 3rd row
        facelets[15 - i] = facelets[3 - i]; // 3rd column becomes the last row
        facelets[3 - i] = temp[i]; // Last column becomes the top row
    }
    // Adjust adjacent face pieces
    // Left -> Front -> Right -> Back
    for (int i = 0; i < 4; i++) {
        char tempLeft = facelets[64 + i];
        facelets[64 + i] = facelets[48 + i];
        facelets[48 + i] = facelets[80 + i];
        facelets[80 + i] = facelets[32 + i];
        facelets[32 + i] = tempLeft;
    }
}

/*
void RubiksCube4x4::move_U(bool clockwise) {
    std::unordered_map<int, int> index_move_map = {
        {48, 80}, {49, 81}, {50, 82}, {51, 83}, {80, 32}, {81, 33}, {82, 34}, {83, 35}, {32, 64}, {33, 65}, {34, 66}, {35, 67}, {64, 48}, {65, 49}, {66, 50}, {67, 51},
        {0, 12}, {1, 8}, {2, 4}, {3, 0}, {4, 13}, {5, 9}, {6, 5}, {7, 1}, {8, 14}, {9, 10}, {10, 6}, {11, 2}, {12, 15}, {13, 11}, {14, 7}, {15, 3}
    };       
    apply_index_swaps(index_move_map, clockwise);
}
*/


void RubiksCube4x4::move_D(bool clockwise) {
    std::unordered_map<int, int> index_move_map = {
        {92, 60}, {93, 61}, {94, 62}, {95, 63}, {44, 92}, {45, 93}, {46, 94}, {47, 95}, {76, 44}, {77, 45}, {78, 46}, {79, 47}, {60, 76}, {61, 77}, {62, 78}, {63, 79},
        {16, 28}, {17, 24}, {18, 20}, {19, 16}, {20, 29}, {21, 25}, {22, 21}, {23, 17}, {24, 30}, {25, 26}, {26, 22}, {27, 18}, {28, 31}, {29, 27}, {30, 23}, {31, 19}
    };        
    apply_index_swaps(index_move_map, clockwise);
}

void RubiksCube4x4::move_F(bool clockwise) {
    std::unordered_map<int, int> index_move_map = {
        {80, 12}, {84, 13}, {88, 14}, {92, 15}, {19, 80}, {18, 84}, {17, 88}, {16, 92}, {79, 19}, {75, 18}, {71, 17}, {67, 16}, {12, 79}, {13, 75}, {14, 71}, {15, 67},
        {48, 60}, {49, 56}, {50, 52}, {51, 48}, {52, 61}, {53, 57}, {54, 53}, {55, 49}, {56, 62}, {57, 58}, {58, 54}, {59, 50}, {60, 63}, {61, 59}, {62, 55}, {63, 51}
    };        
    apply_index_swaps(index_move_map, clockwise);
}

void RubiksCube4x4::move_B(bool clockwise) {
    std::unordered_map<int, int> index_move_map = {
        {64, 3}, {68, 2}, {72, 1}, {76, 0}, {31, 76}, {30, 72}, {29, 68}, {28, 64}, {83, 31}, {87, 30}, {91, 29}, {95, 28}, {0, 83}, {1, 87}, {2, 91}, {3, 95},
        {32, 44}, {33, 40}, {34, 36}, {35, 32}, {36, 45}, {37, 41}, {38, 37}, {39, 33}, {40, 46}, {41, 42}, {42, 38}, {43, 34}, {44, 47}, {45, 43}, {46, 39}, {47, 35},
    };        
    apply_index_swaps(index_move_map, clockwise);
}

