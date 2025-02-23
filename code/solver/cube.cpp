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
    facelets[U_FACE].fill(WHITE);
    facelets[L_FACE].fill(ORANGE);
    facelets[F_FACE].fill(GREEN);
    facelets[R_FACE].fill(RED);
    facelets[B_FACE].fill(BLUE);
    facelets[D_FACE].fill(YELLOW);

    edges_pairs = {0,1,2,3,4,5,6,7,8,9,10,11};
    edges_pairs_parity.fill(false);
    corners = {0,1,2,3,4,5,6,7};
    corners_orientation.fill(0);
    // TODO Update the centres, edges, corners, parity, oreitnations arrays here too.
}

// Export the facelet data as a 96-char string in the order U, L, F, R, B, D
std::string Cube4x4::export_state() const {
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

// Prints visulisation of cube in command line
void Cube4x4::print() const {
    std::cout << "\n\n";
    std::string s = export_state(); 
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

// Visualize runs the ./vis executable
void Cube4x4::visualize() const {
    std::string s = export_state();
    std::string cmd = "./vis \"" + s + "\"";
    std::system(cmd.c_str());
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
std::vector<Move> Cube4x4::apply_random_moves(int n) {
    std::vector<Move> all_moves = {
        R, L, U, D, F, B, R_PRIME, L_PRIME, U_PRIME, D_PRIME, F_PRIME, B_PRIME,
        //r, l, u, d, f, b, r_PRIME, l_PRIME, u_PRIME, d_PRIME, f_PRIME, b_PRIME
    };
    std::srand(std::time(nullptr));
    std::vector<Move> appliedMoves;
    for (int i = 0; i < n; i++) {
        Move randomMove = all_moves[std::rand() % all_moves.size()];
        move(randomMove);
        appliedMoves.push_back(randomMove);
    }

    return appliedMoves;
}


// Checks if the cube is in the solved goal state
bool Cube4x4::check_goal_state() const {
    for (int face = 0; face < 6; ++face) {
        Colour center_color = facelets[face][5];
        for (int i = 0; i < 16; ++i) {
            if (facelets[face][i] != center_color) {
                return false; 
            }
        }
    }
    
    return true;
}


int Cube4x4::misplaced_pieces_heuristic() const {
    // Create a goal cube in solved state
    Cube4x4 goal_cube;

    int mismatch_count = 0;

    // Compare each facelet to the goal state
    for (int face = 0; face < 6; ++face) {
        for (int i = 0; i < 16; ++i) {
            if (facelets[face][i] != goal_cube.facelets[face][i]) {
                mismatch_count++;
            }
        }
    }

    return mismatch_count;
}

int Cube4x4::twist_distance(int phase) const {
    switch (phase) {
        case 1:
            return phase_one_twist_distance();
        case 5:
            return phase_five_twist_distance();
        case 6:
            return phase_six_twist_distance();
        default:
            return -1;
    }
}


int Cube4x4::phase_one_twist_distance() const {
    const std::array<int, 4> center_positions = {5, 6, 9, 10};

    int misplaced = 0;
    for (int pos : center_positions) {
        if (facelets[R_FACE][pos] != RED && facelets[R_FACE][pos] != ORANGE) {
            misplaced++;
        }
        if (facelets[L_FACE][pos] != RED && facelets[L_FACE][pos] != ORANGE) {
            misplaced++;
        }
    }

    return (misplaced + 1) / 2;
}

int Cube4x4::phase_five_twist_distance() const {
    int bad_count = 0;
    for(int i = 0; i < 12; i++){
        if(edges_pairs_parity[i]) {
            bad_count++;
        }
    }
    if (bad_count == 0) {
        return 0;
    }
    return (bad_count + 7) / 8;
}

int Cube4x4::phase_six_twist_distance() const {
    int bad_count = 0;
    if (!(edges_pairs[8] == 8 || edges_pairs[8] == 9 || edges_pairs[8] == 10 || edges_pairs[8] == 11)) {
        bad_count++;
    }
    if (!(edges_pairs[9] == 8 || edges_pairs[9] == 9 || edges_pairs[9] == 10 || edges_pairs[9] == 11)) {
        bad_count++;        
    }
    if (!(edges_pairs[10] == 8 || edges_pairs[10] == 9 || edges_pairs[10] == 10 || edges_pairs[10] == 11)) {
        bad_count++;        
    }
    if (!(edges_pairs[11] == 8 || edges_pairs[11] == 9 || edges_pairs[11] == 10 || edges_pairs[11] == 11)) {
        bad_count++;
    } 

    if (!(facelets[F_FACE][0] == BLUE || facelets[F_FACE][0] == GREEN)) {
        bad_count++;
    } 
    if (!(facelets[F_FACE][3] == BLUE || facelets[F_FACE][3] == GREEN)) {
        bad_count++;
    } 
    if (!(facelets[F_FACE][12] == BLUE || facelets[F_FACE][12] == GREEN)) {
        bad_count++;
    } 
    if (!(facelets[F_FACE][15] == BLUE || facelets[F_FACE][15] == GREEN)) {
        bad_count++;
    } 
    if (!(facelets[B_FACE][0] == BLUE || facelets[B_FACE][0] == GREEN)) {
        bad_count++;
    } 
    if (!(facelets[B_FACE][3] == BLUE || facelets[B_FACE][3] == GREEN)) {
        bad_count++;
    } 
    if (!(facelets[B_FACE][12] == BLUE || facelets[B_FACE][12] == GREEN)) {
        bad_count++;
    } 
    if (!(facelets[B_FACE][15] == BLUE || facelets[B_FACE][15] == GREEN)) {
        bad_count++;
    }


    if (bad_count == 0) {
        return 0;
    }
    return (bad_count + 7) / 8;
}

int Cube4x4::phase_seven_twist_distance() const {
    int bad_count = 0;

    for(int i=0; i<16; i++){
        if (!(facelets[U_FACE][i] == WHITE || facelets[U_FACE][i] == YELLOW)) {
            bad_count++;
        }
    }
    for(int i=0; i<16; i++){
        if (!(facelets[D_FACE][i] == WHITE || facelets[D_FACE][i] == YELLOW)) {
            bad_count++;
        }
    }

    for(int i=0; i<16; i++){
        if (!(facelets[L_FACE][i] == ORANGE || facelets[L_FACE][i] == RED)) {
            bad_count++;
        }
    }
    for(int i=0; i<16; i++){
        if (!(facelets[R_FACE][i] == ORANGE || facelets[R_FACE][i] == RED)) {
            bad_count++;
        }
    }
    
    for(int i=0; i<16; i++){
        if (!(facelets[F_FACE][i] == GREEN || facelets[F_FACE][i] == BLUE)) {
            bad_count++;
        }
    }
    for(int i=0; i<16; i++){
        if (!(facelets[B_FACE][i] == GREEN || facelets[R_FACE][i] == BLUE)) {
            bad_count++;
        }
    }
    
    if (bad_count == 0) {
        return 0;
    }
    return (bad_count + 7) / 8;
}

// Converts a colour enum to a char
char Cube4x4::colour_to_char(Colour c) {
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

// Converts a char to an emoji string
std::string Cube4x4::char_to_emoji(char c) {
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
    const int* edge_cycle = edge_pairs_swaps[face_index];
    const int* corner_cycle = corner_swaps[face_index];
    
    if (face_index == U_FACE || face_index == D_FACE) {
        edges_pairs_parity[edges_pairs[edge_cycle[0]]] = !edges_pairs_parity[edges_pairs[edge_cycle[0]]];
        edges_pairs_parity[edges_pairs[edge_cycle[1]]] = !edges_pairs_parity[edges_pairs[edge_cycle[1]]];
        edges_pairs_parity[edges_pairs[edge_cycle[2]]] = !edges_pairs_parity[edges_pairs[edge_cycle[2]]];
        edges_pairs_parity[edges_pairs[edge_cycle[3]]] = !edges_pairs_parity[edges_pairs[edge_cycle[3]]];
    }
    
    if (clockwise) {
        auto tmp = edges_pairs[edge_cycle[0]];
        edges_pairs[edge_cycle[0]] = edges_pairs[edge_cycle[3]];
        edges_pairs[edge_cycle[3]] = edges_pairs[edge_cycle[2]];
        edges_pairs[edge_cycle[2]] = edges_pairs[edge_cycle[1]];
        edges_pairs[edge_cycle[1]] = tmp;
        tmp = corners[corner_cycle[0]];
        corners[corner_cycle[0]] = corners[corner_cycle[3]];
        corners[corner_cycle[3]] = corners[corner_cycle[2]];
        corners[corner_cycle[2]] = corners[corner_cycle[1]];
        corners[corner_cycle[1]] = tmp;
    } else {
        auto tmp = edges_pairs[edge_cycle[0]];
        edges_pairs[edge_cycle[0]] = edges_pairs[edge_cycle[1]];
        edges_pairs[edge_cycle[1]] = edges_pairs[edge_cycle[2]];
        edges_pairs[edge_cycle[2]] = edges_pairs[edge_cycle[3]];
        edges_pairs[edge_cycle[3]] = tmp;
        tmp = corners[corner_cycle[0]];
        corners[corner_cycle[0]] = corners[corner_cycle[1]];
        corners[corner_cycle[1]] = corners[corner_cycle[2]];
        corners[corner_cycle[2]] = corners[corner_cycle[3]];
        corners[corner_cycle[3]] = tmp;
    }
}

// Turns an inner slice of the cube
void Cube4x4::turn_inner_slice(int face_index, bool clockwise) {
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