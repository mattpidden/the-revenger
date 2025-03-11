#include <array>
#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <map>
#include <cstdlib>  
#include <ctime>
#include <unordered_set>

#include "cube.h"


Cube4x4::Cube4x4() {
    reset();
}

// Reset to goal state:
void Cube4x4::reset() {
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 16; j++) {
            facelets[i][j] = (i * 16) + j;
        }
    }
}

// Export the facelet data as a 96-char string in the order U, L, F, R, B, D
std::string Cube4x4::export_state() const {
    std::string result;
    result.resize(48, '\0');
    for (int i = 0; i < 96; i += 2) {
        int face_index1 = i / 16;
        int pos1 = i % 16;
        int id1 = facelets[face_index1][pos1];
        int code1 = (id1 >= 0) ? (id1 / 16) : (6 + (-id1 - 1));
        
        int face_index2 = (i+1) / 16;
        int pos2 = (i+1) % 16;
        int id2 = facelets[face_index2][pos2];
        int code2 = (id2 >= 0) ? (id2 / 16) : (6 + (-id2 - 1));
        
        unsigned char byte = (static_cast<unsigned char>(code1) << 4) | (static_cast<unsigned char>(code2) & 0x0F);
        result[i / 2] = byte;
    }
    return result;
}

char code_to_char(int code) {
    if (code < 6) {
        return "WOGRBY"[code];
    } else {
        int id = -(code - 5);
        return Cube4x4::id_to_char(id);
    }
}

// Prints visulisation of cube in command line
void Cube4x4::print() const {
    std::cout << "\n\n";
    std::string packed = export_state();
    std::string s;
    s.resize(96, '\0');
    for (int i = 0; i < 48; i++) {
        unsigned char byte = static_cast<unsigned char>(packed[i]);
        int code1 = (byte >> 4) & 0x0F;  
        int code2 = byte & 0x0F;        
        s[2 * i] = code_to_char(code1);
        s[2 * i + 1] = code_to_char(code2);
    }

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
    std::cout << std::endl;
}

// Handles moves
void Cube4x4::move(Move m) {
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

// Applys some random moves
std::vector<Move> Cube4x4::apply_random_moves(int n, std::vector<Move> all_moves) {
    if (all_moves.empty()) {
        all_moves.insert(all_moves.end(), {
        R, L, U, D, F, B, R_PRIME, L_PRIME, U_PRIME, D_PRIME, F_PRIME, B_PRIME,
        r, l, u, d, f, b, r_PRIME, l_PRIME, u_PRIME, d_PRIME, f_PRIME, b_PRIME
        });
    }
    std::srand(std::time(nullptr));
    std::vector<Move> appliedMoves;
    for (int i = 0; i < n; i++) {
        Move randomMove = all_moves[std::rand() % all_moves.size()];
        move(randomMove);
        appliedMoves.push_back(randomMove);
    }

    return appliedMoves;
}

bool Cube4x4::check_solved() const {
    if (export_state() == "WWWWWWWWWWWWWWWWOOOOOOOOOOOOOOOOGGGGGGGGGGGGGGGGRRRRRRRRRRRRRRRRBBBBBBBBBBBBBBBBYYYYYYYYYYYYYYYY") {
        return true;
    }
    return false;
}

// Pass in the pieces you want to keep
void Cube4x4::apply_mask(const std::vector<int>& mask) {
    bool allowed[96] = { false };
    for (int m : mask) {
        allowed[m] = true;
    }
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 16; j++) {
            if (!allowed[facelets[i][j]]) {
                facelets[i][j] = -1;
            }
        }
    }
}

void Cube4x4::apply_colour_mask(int value, const std::vector<int>& mask) {
    bool allowed[96] = { false };
    for (int m : mask) {
        allowed[m] = true;
    }
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 16; j++) {
            if (allowed[facelets[i][j]]) {
                facelets[i][j] = value;
            }
        }
    }
}

void Cube4x4::apply_location_colour_mask(int value, const std::vector<int>& location_mask) {
    bool allowed[96] = { false };
    for (int m : location_mask) {
        allowed[m] = true;
    }
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 16; j++) {
            if (allowed[i * 16 + j]) {
                facelets[i][j] = value;
            }
        }
    }
}

// Converts a facelet id to a char
char Cube4x4::id_to_char(int id) {
    if (id == -1) {  
        return 'X'; 
    } else if (id < 0) {  
        return '0' + (-id);
    }
    return "WOGRBY"[id / 16];
}

// Converts a facelet id to a char
int Cube4x4::char_to_id(char c) {
    switch (c) {
        case 'W': return 0;
        case 'O': return 16;
        case 'G': return 32;
        case 'R': return 48;
        case 'B': return 64;
        case 'Y': return 80;
        case 'X': return -1;
        case '2': return -2;
        case '3': return -3;
        case '4': return -4;
        case '5': return -5;
        default:
            if (c >= '0' && c <= '9') {
                return - (c - '0');
            }
            return -99;
    }
}

// Converts a char to an emoji string
std::string Cube4x4::char_to_emoji(char c) {
    switch(c) {
        case 'W':  return "⬜";
        case 'R':  return "🟥";
        case 'B':  return "🟦";
        case 'O':  return "🟧";
        case 'G':  return "🟩";
        case 'Y':  return "🟨";
        case 'X':  return "⬛";
        case '2':  return "⬜";
        case '3':  return "🟪";
        case '4':  return "🟫";
        default:   return std::string(2, c); 
    }
}

// Turns an outer face of the cube
void Cube4x4::turn_outer_face(int face_index, bool clockwise) {
    auto &face = facelets[face_index];
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

    auto &first_face = facelets[adjacent_faces[face_index][0]];
    const auto &first_edges = adjacent_outer_edges[face_index][0];
    std::array<int, 4> temp_edges = {first_face[first_edges[0]], first_face[first_edges[1]], first_face[first_edges[2]], first_face[first_edges[3]]};
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
}

// Turns an inner slice of the cube
void Cube4x4::turn_inner_slice(int face_index, bool clockwise) {
    auto &first_face = facelets[adjacent_faces[face_index][0]];
    const auto &first_edges = adjacent_inner_edges[face_index][0];
    std::array<int, 4> temp_edges = {first_face[first_edges[0]], first_face[first_edges[1]], first_face[first_edges[2]], first_face[first_edges[3]]};
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

// Converts move enum to a string
std::string move_to_string(Move move) {
    switch (move) {
        case R: return "R"; case R_PRIME: return "R'"; case R2: return "R2";
        case L: return "L"; case L_PRIME: return "L'"; case L2: return "L2";
        case U: return "U"; case U_PRIME: return "U'"; case U2: return "U2";
        case D: return "D"; case D_PRIME: return "D'"; case D2: return "D2";
        case F: return "F"; case F_PRIME: return "F'"; case F2: return "F2";
        case B: return "B"; case B_PRIME: return "B'"; case B2: return "B2";
        case r: return "r"; case r_PRIME: return "r'"; case r2: return "r2";
        case l: return "l"; case l_PRIME: return "l'"; case l2: return "l2";
        case u: return "u"; case u_PRIME: return "u'"; case u2: return "u2";
        case d: return "d"; case d_PRIME: return "d'"; case d2: return "d2";
        case f: return "f"; case f_PRIME: return "f'"; case f2: return "f2";
        case b: return "b"; case b_PRIME: return "b'"; case b2: return "b2";
        default: return "?";  
    }
}

// PHASE 1
std::vector<Move> phase1_moves = {R, L, L_PRIME, L2, F, F_PRIME, F2, B, B_PRIME, B2, U, U_PRIME, U2, D, D_PRIME, D2, r, r_PRIME, r2, l, l_PRIME, l2, f, f_PRIME, f2, b, b_PRIME, b2, u, u_PRIME, u2, d, d_PRIME, d2};
std::function<std::string(Cube4x4&)> phase1_mask = [](Cube4x4 cube) -> std::string {
    std::vector<int> mask = {21,22,25,26,53,54,57,58}; // R centre pieces and L centre pieces
    cube.apply_mask(mask);
    cube.apply_colour_mask(-2, mask);
    return cube.export_state();
};
std::function<bool(const Cube4x4&)> phase1_is_solved = [] (const Cube4x4& cube) {
    Cube4x4 solved_cube;
    return phase1_mask(solved_cube) == cube.export_state();
};
Cube4x4 phase1_cube;
Phase phase1("Phase 1", phase1_moves, phase1_is_solved, phase1_mask, "phase1table.bin", -1, 8);

// PHASE 2
std::vector<Move> phase2_moves = {R, R_PRIME, R2, L, L_PRIME, L2, F, F_PRIME, F2, B, B_PRIME, B2, U, U_PRIME, U2, D, D_PRIME, D2, r, r_PRIME, r2, l, l_PRIME, l2, f2, b2, u2, d2};
std::function<std::string(Cube4x4&)> phase2_mask = [](Cube4x4 cube) -> std::string {
    std::vector<int> mask = {37,38,41,42,69,70,73,74,21,22,25,26,53,54,57,58,5,6,9,10,85,86,89,90, 13,93,4,2,11,84,82,91,52,59,27,20,14,94,8,1,7,88,81,87,56,55,23,24};
    std::vector<int> colour_mask1 = {5,6,9,10,85,86,89,90}; // U and D centre pieces
    std::vector<int> colour_mask2 = {37,38,41,42,69,70,73,74}; // F and B centre pieces
    std::vector<int> high_edges = {14,94,8,1,7,88,81,87,56,55,23,24};
    std::vector<int> low_edges = {13,93,4,2,11,84,82,91,52,59,27,20};
    cube.apply_mask(mask);
    cube.apply_colour_mask(0, colour_mask1);
    cube.apply_colour_mask(32, colour_mask2);
    cube.apply_colour_mask(-3, high_edges);
    cube.apply_colour_mask(-4, low_edges);
    return cube.export_state();
};
std::function<bool(const Cube4x4&)> phase2_is_solved = [] (const Cube4x4& cube) {
    Cube4x4 solved_cube;
    return phase2_mask(solved_cube) == cube.export_state();
};
Cube4x4 phase2_cube;
Phase phase2("Phase 2", phase2_moves, phase2_is_solved, phase2_mask, "phase2table.bin", 6, 15);


// PHASE 3
std::vector<Move> phase3_moves = {R2, L2, F, F_PRIME, F2, B, B_PRIME, B2, U, U_PRIME, U2, D, D_PRIME, D2, r2, l2, f2, b2, u2, d2};
std::function<std::string(Cube4x4&)> phase3_mask = [](Cube4x4 cube) -> std::string {
    // this mask takes a long time to compute
    std::vector<int> mask = {37,38,41,42,69,70,73,74,21,22,25,26,53,54,57,58}; // all centres pieces on R,L,F,B faces
    std::vector<int> edge_facelets = {20,24,23,27,36,40,39,43,52,56,55,59,68,72,71,75}; // all edge facelets in middle slice
    std::vector<int> paired_facelets = {};
    paired_facelets.reserve(edge_facelets.size());
    std::vector<int> unpaired_facelets = {};
    unpaired_facelets.reserve(edge_facelets.size());
    for (size_t i = 0; i < edge_facelets.size(); i += 2) {
        int pos1 = edge_facelets[i];
        int pos2 = edge_facelets[i + 1];
        if (cube.id_to_char(cube.facelets[pos1 / 16][pos1 % 16]) == cube.id_to_char(cube.facelets[pos2 / 16][pos2 % 16])) {
            paired_facelets.push_back(pos1);
            paired_facelets.push_back(pos2);            
        } else {
            unpaired_facelets.push_back(pos1);
            unpaired_facelets.push_back(pos2);      
        }
    }
    cube.apply_mask(mask);
    cube.apply_location_colour_mask(-3, paired_facelets);
    cube.apply_location_colour_mask(-4, unpaired_facelets);
    return cube.export_state();
};
std::function<bool(const Cube4x4&)> phase3_is_solved = [] (const Cube4x4& cube) {
    Cube4x4 solved_cube;
    return phase3_mask(solved_cube) == cube.export_state();
};
Cube4x4 phase3_cube;
Phase phase3("Phase 3", phase3_moves, phase3_is_solved, phase3_mask, "phase3table.bin", -1, 13);


// PHASE 4
std::vector<Move> phase4_moves = {R2, L2, F2, B2, U, U_PRIME, U2, D, D_PRIME, D2, r2, l2, f2, b2};
std::function<std::string(Cube4x4&)> phase4_mask = [](Cube4x4 cube) -> std::string {
    // this mask takes a really long time to compute
    std::vector<int> mask = {5,6,9,10,85,86,89,90,37,38,41,42,69,70,73,74,21,22,25,26,53,54,57,58}; // all centre pieces
    std::vector<int> edge_facelets = {1,2,4,8,7,11,13,14,17,18,33,34,49,50,65,66,29,30,45,46,61,62,77,78,81,82,84,88,87,91,93,94}; // all edge facelets except those in middle slice
    std::vector<int> paired_facelets = {};
    paired_facelets.reserve(edge_facelets.size());
    std::vector<int> unpaired_facelets = {};
    unpaired_facelets.reserve(edge_facelets.size());
    for (size_t i = 0; i < edge_facelets.size(); i += 2) {
        int pos1 = edge_facelets[i]; 
        int pos2 = edge_facelets[i + 1];
        if (cube.id_to_char(cube.facelets[pos1 / 16][pos1 % 16]) == cube.id_to_char(cube.facelets[pos2 / 16][pos2 % 16])) {
            paired_facelets.push_back(pos1);
            paired_facelets.push_back(pos2);            
        } else {
            unpaired_facelets.push_back(pos1);
            unpaired_facelets.push_back(pos2);      
        }
    }
    cube.apply_mask(mask);
    cube.apply_location_colour_mask(-3, paired_facelets);
    cube.apply_location_colour_mask(-4, unpaired_facelets);
    return cube.export_state();
};
std::function<bool(const Cube4x4&)> phase4_is_solved = [] (const Cube4x4& cube) {
    Cube4x4 solved_cube;
    return phase4_mask(solved_cube) == cube.export_state();
};
Cube4x4 phase4_cube;
Phase phase4("Phase 4", phase4_moves, phase4_is_solved, phase4_mask, "phase4table.bin", -1, 25);


// PHASE 5
std::vector<Move> phase5_moves = {R, R_PRIME, R2, L, L_PRIME, L2, F, F_PRIME, F2, B, B_PRIME, B2, U, U_PRIME, U2, D, D_PRIME, D2};
std::function<std::string(Cube4x4&)> phase5_mask = [](Cube4x4 cube) -> std::string {
    std::vector<int> mask = {1,2,4,7,8,11,13,14,36,39,40,43,68,71,72,75,81,82,84,87,88,91,93,94}; // 'good' edges facelets
    cube.apply_mask(mask);
    cube.apply_colour_mask(-2, mask);
    return cube.export_state();
};
std::function<bool(const Cube4x4&)> phase5_is_solved = [] (const Cube4x4& cube) {
    Cube4x4 solved_cube;
    return phase5_mask(solved_cube) == cube.export_state();
};
Cube4x4 phase5_cube;
Phase phase5("Phase 5", phase5_moves, phase5_is_solved, phase5_mask, "phase5table.bin", -1, 7);

// PHASE 6
std::vector<Move> phase6_moves = {R, R_PRIME, R2, L, L_PRIME, L2, F2, B2, U, U_PRIME, U2, D, D_PRIME, D2};
std::function<std::string(Cube4x4&)> phase6_mask = [](Cube4x4 cube) -> std::string {
    std::vector<int> mask = {0,1,2,3,4,7,8,11,12,13,14,15,36,39,40,43,68,71,72,75,80,81,82,83,84,87,88,91,92,93,94,95};
    std::vector<int> colour_mask1 = {0,1,2,3,4,7,8,11,12,13,14,15,80,81,82,83,84,87,88,91,92,93,94,95}; 
    std::vector<int> colour_mask2 = {36,39,40,43,68,71,72,75};
    cube.apply_mask(mask);
    cube.apply_colour_mask(-3, colour_mask1);
    cube.apply_colour_mask(-4, colour_mask2);
    return cube.export_state();
};
std::function<bool(const Cube4x4&)> phase6_is_solved = [] (const Cube4x4& cube) {
    Cube4x4 solved_cube;    
    return phase6_mask(solved_cube) == cube.export_state();
};
Cube4x4 phase6_cube;
Phase phase6("Phase 6", phase6_moves, phase6_is_solved, phase6_mask, "phase6table.bin", -1, 10);

// PHASE 7
std::vector<Move> phase7_moves = {R2, L2, F2, B2, U, U_PRIME, U2, D, D_PRIME, D2};
std::function<std::string(Cube4x4&)> phase7_mask = [](Cube4x4 cube) -> std::string {
    std::vector<int> mask = {0,3,12,15,16,17,18,19,20,23,24,27,28,29,30,31,32,33,34,35,36,39,40,43,44,45,46,47,48,49,50,51,52,55,56,59,60,61,62,63,64,65,66,67,68,71,72,75,76,77,78,79,80,83,92,95};
    std::vector<int> colour_mask1 = {17,18,20,23,24,27,29,30,49,50,52,55,56,59,61,62};
    std::vector<int> colour_mask2 = {33,34,36,39,40,43,45,46,65,66,68,71,72,75,77,78};
    cube.apply_mask(mask);
    cube.apply_colour_mask(-3, colour_mask1);
    cube.apply_colour_mask(-4, colour_mask2);
    return cube.export_state();
};
std::function<bool(const Cube4x4&)> phase7_is_solved = [] (const Cube4x4& cube) {
    Cube4x4 solved_cube;    
    return phase7_mask(solved_cube) == cube.export_state();
};
Phase phase7("Phase 7", phase7_moves, phase7_is_solved, phase7_mask, "phase7table.bin", -1, 13);

// PHASE 8
std::vector<Move> phase8_moves = {R2, L2, F2, B2, U2, D2};
std::function<std::string(Cube4x4&)> phase8_mask = [](Cube4x4 cube) -> std::string {
    std::vector<int> mask = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95};
    cube.apply_mask(mask);
    return cube.export_state();
};
std::function<bool(const Cube4x4&)> phase8_is_solved = [] (const Cube4x4& cube) {
    Cube4x4 solved_cube;    
    return solved_cube.export_state() == cube.export_state();
};
Cube4x4 phase8_cube;
Phase phase8("Phase 8", phase8_moves, phase8_is_solved, phase8_mask, "phase8table.bin", -1, 15);