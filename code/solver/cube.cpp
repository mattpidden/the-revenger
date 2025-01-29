#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <sstream> 

class RubiksCube4x4 {
private:
    std::vector<char> facelets;

public:
    RubiksCube4x4() {
        facelets = {
            'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W',  // Up face  
            'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y',  // Down face
            'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B',  // Back face
            'G', 'G', 'G', 'G', 'G', 'G', 'G', 'G', 'G', 'G', 'G', 'G', 'G', 'G', 'G', 'G',  // Front face
            'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O',  // Left face
            'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R',    // Right face
        };
    }

    // Method to display the cube's facelets
    void displayCube() {
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
    }

    void apply_moves(const std::string& moves) {
        std::istringstream iss(moves);
        std::string move;
        
        while (iss >> move) {
            apply_move(move);
        }
    }

    void apply_move(const std::string& move) {
        bool clockwise = (move.back() != '\''); 

        std::string base_move = move;
        if (!clockwise) base_move = move.substr(0, move.size() - 1); // Remove ' if present
        int repeat = (base_move.back() == '2') ? 2 : 1;
        if (repeat == 2) base_move = base_move.substr(0, base_move.size() - 1); // Remove '2' if present

        for (int i = 0; i < repeat; i++) {
            if (base_move == "R") move_R(clockwise);
            else if (base_move == "r") move_r(clockwise);
            else if (base_move == "Rw") move_Rw(clockwise);
            else std::cout << "Invalid move: " << move << "\n";
        }
    }

    void apply_index_swaps(const std::unordered_map<int, int>& index_move_map, bool clockwise) {
        std::vector<char> facelets_copy = facelets;
        for (const auto& [a, b] : index_move_map) {
            if (clockwise) {
                facelets[a] = facelets_copy[b];
            } else {
                facelets[b] = facelets_copy[a];
            }
        }
    }

    void move_R(bool clockwise) {
        std::unordered_map<int, int> index_move_map = {
            {3, 51}, {7, 55}, {11, 59}, {15, 63}, {51, 19}, {55, 23}, {59, 27}, {63, 31}, {19, 44}, {23,40}, {27, 36}, {31, 32}, {44, 3}, {40, 7}, {36, 11}, {32,15},
            {80, 92}, {81, 88}, {82, 84}, {83, 80}, {84, 93}, {85, 89}, {86, 85}, {87, 81}, {88, 94}, {89, 90}, {90, 86}, {91, 82}, {92, 95}, {93, 91}, {94, 87}, {95, 83}
        };       
        apply_index_swaps(index_move_map, clockwise);
    }

    void move_r(bool clockwise) {
        std::unordered_map<int, int> index_move_map = {
            {2, 50}, {6, 54}, {10, 58}, {14, 62}, {50, 18}, {54, 22}, {58, 26}, {62, 30}, {18, 45}, {22,41}, {26, 37}, {30, 33}, {45, 2}, {41, 6}, {37, 10}, {33,14},
        };       
        apply_index_swaps(index_move_map, clockwise);
    }

    void move_Rw(bool clockwise){
        move_r(clockwise);
        move_R(clockwise);
    }

};

int main() {
    RubiksCube4x4 cube;

    cube.apply_moves("R' R' R2");

    cube.displayCube();

    return 0;
}
