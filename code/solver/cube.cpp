#include <iostream>
#include <vector>
#include <string>
#include <map>

class RubiksCube4x4 {
private:
    // Vector to store the 96 facelet colors
    std::vector<char> facelets;

public:
    RubiksCube4x4() {
        facelets = {
            // Up face 
            'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W',
            // Down face
            'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y',
            // Back face
            'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B',
            // Front face
            'G', 'G', 'G', 'G', 'G', 'G', 'G', 'G', 'G', 'G', 'G', 'G', 'G', 'G', 'G', 'G',
            // Left face
            'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O',
            // Right face
            'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R'
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

    void moveR() {
        // Rotate the right face clockwise
        std::vector<char> newRight(16);
        newRight[0] = facelets[92]; newRight[1] = facelets[88]; newRight[2] = facelets[84]; newRight[3] = facelets[80];
        newRight[4] = facelets[93]; newRight[5] = facelets[89]; newRight[6] = facelets[85]; newRight[7] = facelets[81];
        newRight[8] = facelets[94]; newRight[9] = facelets[90]; newRight[10] = facelets[86]; newRight[11] = facelets[82];
        newRight[12] = facelets[95]; newRight[13] = facelets[91]; newRight[14] = facelets[87]; newRight[15] = facelets[83];
        for (int i = 0; i < 16; ++i) {
            facelets[80 + i] = newRight[i];
        }
        // Cycle adjacent facelets (U, F, D, B)
        std::vector<char> temp = {
            facelets[3], facelets[7], facelets[11], facelets[15] // Last column of U
        };
        // U = F
        for (int i = 0; i < 4; ++i) {
            facelets[0 + i * 4 + 3] = facelets[48 + i * 4 + 3];
        }
        // F = D
        for (int i = 0; i < 4; ++i) {
            facelets[48 + i * 4 + 3] = facelets[16 + i * 4 + 3];
        }
        // D = B
        for (int i = 0; i < 4; ++i) {
            facelets[16 + i * 4 + 3] = facelets[44 - i * 4];
        }
        // B = U
        for (int i = 0; i < 4; ++i) {
            facelets[32 + i * 4] = temp[3 - i];
        }
    }

};

int main() {
    RubiksCube4x4 cube;

    cube.moveR();

    cube.displayCube();

    return 0;
}
