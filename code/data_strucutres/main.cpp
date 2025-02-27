#include <array>
#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <map>
#include <chrono>
#include <malloc.h>

class Cube4x4 {
private:
    std::vector<char> facelets;

public:
    Cube4x4() {
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
        std::vector<char> facelets = facelets;
        for (const auto& [a, b] : index_move_map) {
            if (clockwise) {
                facelets[a] = facelets[b];
            } else {
                facelets[b] = facelets[a];
            }
        }
    }

    void move_R(bool clockwise) {
        auto temp3 = facelets[3];
        auto temp7 = facelets[3];
        auto temp11 = facelets[11];
        auto temp15 = facelets[15];
        facelets[3] = facelets[51];
        facelets[7] = facelets[55];
        facelets[11] = facelets[59];
        facelets[15] = facelets[63];
        facelets[51] = facelets[19];
        facelets[55] = facelets[23];
        facelets[59] = facelets[27];
        facelets[63] = facelets[31];
        facelets[19] = facelets[44];
        facelets[23] = facelets[40];
        facelets[27] = facelets[36];
        facelets[31] = facelets[32];
        facelets[44] = temp3;
        facelets[40] = temp7;
        facelets[36] = temp11;
        facelets[32] = temp15;
        auto temp80 = facelets[80];
        auto temp81 = facelets[81];
        auto temp82 = facelets[82];
        auto temp83 = facelets[83];
        auto temp85 = facelets[85];
        auto temp87 = facelets[87];
        auto temp91 = facelets[91];
        facelets[80] = facelets[92];
        facelets[81] = facelets[88];
        facelets[82] = facelets[84];
        facelets[83] = temp80;
        facelets[84] = facelets[93];
        facelets[85] = facelets[89];
        facelets[86] = temp85;
        facelets[87] = temp81;
        facelets[88] = facelets[94];
        facelets[89] = facelets[90];
        facelets[90] = facelets[86];
        facelets[91] = temp82;
        facelets[92] = facelets[95];
        facelets[93] = temp91;
        facelets[94] = temp87;
        facelets[95] = temp83;
    }

    void move_r(bool clockwise) {   
        auto temp2 = facelets[2];
        auto temp6 = facelets[6];
        auto temp10 = facelets[10];
        auto temp14 = facelets[14];
        facelets[2] = facelets[50];
        facelets[6] = facelets[54];
        facelets[10] = facelets[58];
        facelets[14] = facelets[62];
        facelets[50] = facelets[18];
        facelets[54] = facelets[22];
        facelets[58] = facelets[26];
        facelets[62] = facelets[30];
        facelets[18] = facelets[45];
        facelets[22] = facelets[41];
        facelets[26] = facelets[37];
        facelets[30] = facelets[33];
        facelets[45] = temp2;
        facelets[41] = temp6;
        facelets[37] = temp10;
        facelets[33] = temp14;
    }

    void move_Rw(bool clockwise){
        move_r(clockwise);
        move_R(clockwise);
    }

};

int main() {
    Cube4x4 cube;
    int n = 1000000;

    // Benchmark speed
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < n; i++) {
        cube.apply_move("R");
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;
    double outter_time_ms = duration.count();
    double outter_moves_per_sec = (n / outter_time_ms) * 1000.0 / 1e6;
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < n; i++) {
        cube.apply_move("r");
    }
    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    double inner_time_ms = duration.count();
    double inner_moves_per_sec = (n / inner_time_ms) * 1000.0 / 1e6;

    // Benchmark memory
    size_t cube_size = sizeof(cube);
    struct mallinfo before = mallinfo();
    std::vector<Cube4x4*> cubes;
    cubes.reserve(n);
    for (int i = 0; i < n; i++) {
        cubes.push_back(new Cube4x4());
    }
    struct mallinfo after = mallinfo(); 
    size_t actual_memory_used = after.uordblks - before.uordblks;
    for (auto cube : cubes) {
        delete cube;
    }

    std::cout << "1D char vector [96]\n";
    std::cout << "Time taken for " << n << " outer slice moves: " << outter_time_ms << " ms.\n";
    std::cout << "Outer Slice Moves (millions) per second: " << outter_moves_per_sec << ".\n";
    std::cout << "Time taken for " << n << " inner slice moves: " << inner_time_ms << " ms.\n";
    std::cout << "Inner Slice Moves (millions) per second: " << inner_moves_per_sec << ".\n";
    std::cout << "Size of one Cube4x4 instance: " << cube_size << " bytes\n";
    std::cout << "Heap memory used for " << n << " cubes: " << actual_memory_used / (1024.0 * 1024.0) << " MB\n";

    return 0;
} 
