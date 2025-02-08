#include <array>
#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <map>

enum Colour {
    WHITE = 0, YELLOW, GREEN, BLUE, ORANGE, RED, 
};

enum Face {
    U_FACE = 0,
    L_FACE,
    F_FACE,
    R_FACE,
    B_FACE,
    D_FACE
};

enum Move {
    R, R_PRIME, R2,
    L, L_PRIME, L2,
    U, U_PRIME, U2,
    D, D_PRIME, D2,
    F, F_PRIME, F2,
    B, B_PRIME, B2,
    r, r_PRIME, r2,
    l, l_PRIME, l2,
    u, u_PRIME, u2,
    d, d_PRIME, d2,
    f, f_PRIME, f2,
    b, b_PRIME, b2,
};

static char colour_to_char(Colour c) {
    switch(c) {
        case WHITE:  return 'W';
        case RED:    return 'R';
        case BLUE:   return 'B';
        case ORANGE: return 'O';
        case GREEN:  return 'G';
        case YELLOW: return 'Y';
        default:     return '?';
    }  
}

static std::string char_to_emoji(char c) {
    switch(c) {
        case 'W':  return "⬜";
        case 'R':  return "🟥";
        case 'B':  return "🟦";
        case 'O':  return "🟧";
        case 'G':  return "🟩";
        case 'Y':  return "🟨";
        default:   return "?";
    }
}

class Cube4x4 {
public:
    // Facelets: 6 faces × 16 stickers
    // facelets[U-FACE][0..15], facelets[R_FACE][0..15], ...
    std::array<std::array<Colour, 16>, 6> facelets;

    // 24 centers with int IDs
    std::array<int,24> centers;  

    // 24 edge pieces with int IDs
    std::array<int,24> edges;   
    std::array<int, 24> edge_flips;
    std::array<int,12> edges_pairs;   
    std::array<int,12> edges_pairs_parity;   

    // 8 corners with int IDs plus orientation array (0..2).
    std::array<int,8> corners;
    std::array<int,8> corners_orientation;

    // Constructor: make a solved cube
    Cube4x4() {
        reset();
    }

    // Reset to goal state:
    void reset() {
        facelets[U_FACE].fill(WHITE);
        facelets[L_FACE].fill(ORANGE);
        facelets[F_FACE].fill(GREEN);
        facelets[R_FACE].fill(RED);
        facelets[B_FACE].fill(BLUE);
        facelets[D_FACE].fill(YELLOW);

        edge_flips.fill(0);
        // TODO Update the centres, edges, corners, parity, oreitnations arrays here too.
    }

    // Export the facelet data as a 96-char string in the order U, L, F, R, B, D
    std::string exportState() const {
        std::string result;
        result.reserve(96);
        auto appendFace = [&](Face f) {
            for(int i=0; i<16; i++){
                result.push_back( colour_to_char(facelets[f][i]) );
            }
        };
        appendFace(U_FACE);
        appendFace(L_FACE);
        appendFace(F_FACE);
        appendFace(R_FACE);
        appendFace(B_FACE);
        appendFace(D_FACE);
        return result;
    }

    // Print: Prints visulisation of cube in command line
    void print() const {
        std::string s = exportState(); 
        std::cout << "          Up (U)\n";
        for (int row = 0; row < 4; ++row) {
            std::cout << "          ";
            for (int col = 0; col < 4; ++col) {
                std::cout << char_to_emoji(s[0 + row * 4 + col]);

            }
            std::cout << "\n";
        }
        std::cout << "\nLeft (L)  Front (F) Right (R) Back (B)\n";
        for (int row = 0; row < 4; ++row) {
            for (int col = 0; col < 4; ++col)
                std::cout << char_to_emoji(s[16 + row * 4 + col]);
            std::cout << "  ";
            for (int col = 0; col < 4; ++col)
                std::cout << char_to_emoji(s[32 + row * 4 + col]);
            std::cout << "  ";
            for (int col = 0; col < 4; ++col)
                std::cout << char_to_emoji(s[48 + row * 4 + col]);
            std::cout << "  ";
            for (int col = 0; col < 4; ++col)
                std::cout << char_to_emoji(s[64 + row * 4 + col]);
            std::cout << "\n";
        }
        std::cout << "\n          Down (D)\n";
        for (int i = 0; i < 4; ++i) {
            std::cout << "          ";
            for (int j = 0; j < 4; ++j) {
                std::cout << char_to_emoji(s[80 + i * 4 + j]);
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }

    // Visualize: run the ./vis executable
    void visualize() const {
        std::string s = exportState();
        std::string cmd = "./vis \"" + s + "\"";
        std::system(cmd.c_str());
    }

    // Handles moves
    void move(Move m) {
        switch(m) {
            case U:  turn_outer_face(U_FACE, true); break;
            case U_PRIME: turn_outer_face(U_FACE, false); break;
            case U2: turn_outer_face(U_FACE, true); turn_outer_face(U_FACE, true); break;
            case L:  turn_outer_face(L_FACE, true); break;
            case L_PRIME: turn_outer_face(L_FACE, false); break;
            case L2: turn_outer_face(L_FACE, true); turn_outer_face(L_FACE, true); break;
            case F:  turn_outer_face(F_FACE, true); break;
            case F_PRIME: turn_outer_face(F_FACE, false); break;
            case F2: turn_outer_face(F_FACE, true); turn_outer_face(F_FACE, true); break;
            case R:  turn_outer_face(R_FACE, true); break;
            case R_PRIME: turn_outer_face(R_FACE, false); break;
            case R2: turn_outer_face(R_FACE, true); turn_outer_face(R_FACE, true); break;
            case B:  turn_outer_face(B_FACE, true); break;
            case B_PRIME: turn_outer_face(B_FACE, false); break;
            case B2: turn_outer_face(B_FACE, true); turn_outer_face(B_FACE, true); break;
            case D:  turn_outer_face(D_FACE, true); break;
            case D_PRIME: turn_outer_face(D_FACE, false); break;
            case D2: turn_outer_face(D_FACE, true); turn_outer_face(D_FACE, true); break;
            case u:  turn_inner_slice(U_FACE, true); break;
            case u_PRIME: turn_inner_slice(U_FACE, false); break;
            case u2: turn_inner_slice(U_FACE, true); turn_inner_slice(U_FACE, true); break;
            case l:  turn_inner_slice(L_FACE, true); break;
            case l_PRIME: turn_inner_slice(L_FACE, false); break;
            case l2: turn_inner_slice(L_FACE, true); turn_inner_slice(L_FACE, true); break;
            case f:  turn_inner_slice(F_FACE, true); break;
            case f_PRIME: turn_inner_slice(F_FACE, false); break;
            case f2: turn_inner_slice(F_FACE, true); turn_inner_slice(F_FACE, true); break;
            case r:  turn_inner_slice(R_FACE, true); break;
            case r_PRIME: turn_inner_slice(R_FACE, false); break;
            case r2: turn_inner_slice(R_FACE, true); turn_inner_slice(R_FACE, true); break;
            case b:  turn_inner_slice(B_FACE, true); break;
            case b_PRIME: turn_inner_slice(B_FACE, false); break;
            case b2: turn_inner_slice(B_FACE, true); turn_inner_slice(B_FACE, true); break;
            case d:  turn_inner_slice(D_FACE, true); break;
            case d_PRIME: turn_inner_slice(D_FACE, false); break;
            case d2: turn_inner_slice(D_FACE, true); turn_inner_slice(D_FACE, true); break;
            default:
                break;
        }
    }

    bool check_edge_pair_parity() {
        for (int i = 0; i < 24; i += 2) {
            if (edge_flips[i] != edge_flips[i + 1]) {
                return false; 
            }
        }
        return true; 
    }

private:
    static constexpr int adjacent_faces[6][4] = {
        {L_FACE, B_FACE, R_FACE, F_FACE},  // U (Up)
        {U_FACE, F_FACE, D_FACE, B_FACE},  // L (Left)
        {U_FACE, R_FACE, D_FACE, L_FACE},  // F (Front)
        {U_FACE, B_FACE, D_FACE, F_FACE},  // R (Right)
        {U_FACE, L_FACE, D_FACE, R_FACE},  // B (Back)
        {L_FACE, F_FACE, R_FACE, B_FACE}   // D (Down)
    };

    static constexpr int adjacent_outer_edges[6][4][4] = {
        {{0,1,2,3}, {0,1,2,3}, {0,1,2,3}, {0,1,2,3}},        // U (Up)
        {{0,4,8,12}, {0,4,8,12}, {0,4,8,12}, {15,11,7,3}},  // L (Left)
        {{12,13,14,15}, {0,4,8,12}, {3,2,1,0}, {15,11,7,3}},  // F (Front)
        {{3,7,11,15}, {12,8,4,0}, {3,7,11,15}, {3,7,11,15}},  // R (Right)
        {{0,1,2,3}, {12,8,4,0}, {15,14,13,12}, {3,7,11,15}},  // B (Back)
        {{12,13,14,15}, {12,13,14,15}, {12,13,14,15}, {12,13,14,15}}   // D (Down)
    };

    static constexpr int adjacent_inner_edges[6][4][4] = {
        {{4,5,6,7}, {4,5,6,7}, {4,5,6,7}, {4,5,6,7}},        // U (Up)
        {{1,5,9,13}, {1,5,9,13}, {1,5,9,13}, {14,10,6,2}},  // L (Left)
        {{8,9,10,11}, {1,5,9,13}, {7,6,5,4}, {14,10,6,2}},  // F (Front)
        {{2,6,10,14}, {13,9,5,1}, {2,6,10,14}, {2,6,10,14}},  // R (Right)
        {{4,5,6,7}, {13,9,5,1}, {11,10,9,8}, {2,6,10,14}},  // B (Back)
        {{8,9,10,11}, {8,9,10,11}, {8,9,10,11}, {8,9,10,11}}   // D (Down)
    };


    void turn_outer_face(int face_index, bool clockwise) {
        auto &face = facelets[face_index];

        // Rotate the face itself
        if (clockwise) {
            auto tmp = face[0]; face[0] = face[12];  face[12] = face[15];  face[15] = face[3];  face[3] = tmp;
            tmp = face[1]; face[1] = face[8];  face[8] = face[14];  face[14] = face[7];  face[7] = tmp;
            tmp = face[2]; face[2] = face[4];  face[4] = face[13];  face[13] = face[11];  face[11] = tmp;
            tmp = face[5]; face[5] = face[9];  face[9] = face[10];  face[10] = face[6];  face[6] = tmp;
        } else {
            auto tmp = face[0]; face[0] = face[3];  face[3] = face[15];  face[15] = face[12];  face[12] = tmp;
            tmp = face[1]; face[1] = face[7];  face[7] = face[14];  face[14] = face[8];  face[8] = tmp;
            tmp = face[2]; face[2] = face[11];  face[11] = face[13];  face[13] = face[4];  face[4] = tmp;
            tmp = face[5]; face[5] = face[6];  face[6] = face[10];  face[10] = face[9];  face[9] = tmp;
        }

        // Rotate the adjacent edges
        auto &first_face = facelets[adjacent_faces[face_index][0]];
        const auto &first_edges = adjacent_outer_edges[face_index][0];
        std::array<Colour, 4> temp_edges = {first_face[first_edges[0]], first_face[first_edges[1]], first_face[first_edges[2]], first_face[first_edges[3]]};
        int start_index = clockwise ? 3 : 1;
        int step = clockwise ? -1 : 1;
        for (int i = start_index; (clockwise ? i > 0 : i < 4); i += step) {
            auto &current_face = facelets[adjacent_faces[face_index][i]];
            const auto &current_edges = adjacent_outer_edges[face_index][i];
            int next_index = clockwise ? (i + 1) % 4 : (i - 1 + 4) % 4;
            auto &next_face = facelets[adjacent_faces[face_index][next_index]];
            const auto &next_edges = adjacent_outer_edges[face_index][next_index];
            next_face[next_edges[0]] = current_face[current_edges[0]];
            next_face[next_edges[1]] = current_face[current_edges[1]];
            next_face[next_edges[2]] = current_face[current_edges[2]];
            next_face[next_edges[3]] = current_face[current_edges[3]];
        }
        int last_face_index = clockwise ? 1 : 3;
        auto &last_face = facelets[adjacent_faces[face_index][last_face_index]];
        const auto &lastEdges = adjacent_outer_edges[face_index][last_face_index];
        last_face[lastEdges[0]] = temp_edges[0];  
        last_face[lastEdges[1]] = temp_edges[1];  
        last_face[lastEdges[2]] = temp_edges[2];  
        last_face[lastEdges[3]] = temp_edges[3];  

        // TODO Finish this function, including updating centres, edges, corners, paritys, orientation arrays
    }

    void turn_inner_slice(int face_index, bool clockwise) {
        auto &first_face = facelets[adjacent_faces[face_index][0]];
        const auto &first_edges = adjacent_inner_edges[face_index][0];
        std::array<Colour, 4> temp_edges = {first_face[first_edges[0]], first_face[first_edges[1]], first_face[first_edges[2]], first_face[first_edges[3]]};
        int start_index = clockwise ? 3 : 1;
        int step = clockwise ? -1 : 1;
        for (int i = start_index; (clockwise ? i > 0 : i < 4); i += step) {
            auto &current_face = facelets[adjacent_faces[face_index][i]];
            const auto &current_edges = adjacent_inner_edges[face_index][i];
            int next_index = clockwise ? (i + 1) % 4 : (i - 1 + 4) % 4;
            auto &next_face = facelets[adjacent_faces[face_index][next_index]];
            const auto &next_edges = adjacent_inner_edges[face_index][next_index];
            next_face[next_edges[0]] = current_face[current_edges[0]];
            next_face[next_edges[1]] = current_face[current_edges[1]];
            next_face[next_edges[2]] = current_face[current_edges[2]];
            next_face[next_edges[3]] = current_face[current_edges[3]];
        }
        int last_face_index = clockwise ? 1 : 3;
        auto &last_face = facelets[adjacent_faces[face_index][last_face_index]];
        const auto &lastEdges = adjacent_inner_edges[face_index][last_face_index];
        last_face[lastEdges[0]] = temp_edges[0];  
        last_face[lastEdges[1]] = temp_edges[1];  
        last_face[lastEdges[2]] = temp_edges[2];  
        last_face[lastEdges[3]] = temp_edges[3];  

        // TODO Finish this function, including updating centres, edges, corners, paritys, orientation arrays
    }
};



int main() {
    Cube4x4 cube;
    cube.move(F);
    cube.move(R);

    cube.print();

    return 0;
}
