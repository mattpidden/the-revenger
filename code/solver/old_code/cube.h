#ifndef CUBE_H
#define CUBE_H

#include <array>
#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>
#include <map>

enum Colour {
    WHITE = 0, YELLOW, GREEN, BLUE, ORANGE, RED
};

enum Face {
    U_FACE = 0, L_FACE, F_FACE, R_FACE, B_FACE, D_FACE
};

enum Move {
    R, R_PRIME, R2, L, L_PRIME, L2,
    U, U_PRIME, U2, D, D_PRIME, D2,
    F, F_PRIME, F2, B, B_PRIME, B2,
    r, r_PRIME, r2, l, l_PRIME, l2,
    u, u_PRIME, u2, d, d_PRIME, d2,
    f, f_PRIME, f2, b, b_PRIME, b2
};

class Cube4x4 {
public:
    Cube4x4();  
 
    void reset();  
    std::string export_state() const;  
    void import_state(const std::string& state);
    void print() const;  
    void visualize() const; 
    void move(Move m); 
    std::vector<Move> apply_random_moves(int n,  std::vector<Move> all_moves);
    bool check_goal_state() const;  
    bool check_solved() const;
    int misplaced_pieces_heuristic() const;

    int twist_distance(int phase) const;
    int phase_one_twist_distance() const;
    int phase_five_twist_distance() const;
    int phase_six_twist_distance() const;
    int phase_seven_twist_distance() const;
    int white_centre_twist_distance() const;
    int yellow_centre1_twist_distance() const;
    int yellow_centre2_twist_distance() const;
    int orange_centre_twist_distance() const;
    int red_centre_twist_distance() const;
    int blue_green_centre_twist_distance() const;
    int first_edge_twist_distance(int number_edges) const;
    int cross_twist_distance() const;
    int f2l_twist_distance(int number_pairs) const;

private:
    std::array<std::array<Colour, 16>, 6> facelets;
    std::array<int, 96> indexed_facelets;

    std::array<int,24> centers;  
    std::array<int,24> edges;   
    std::array<int, 24> edge_flips;
    std::array<int,12> edges_pairs;   
    std::array<bool,12> edges_pairs_parity;   
    std::array<int,8> corners;
    std::array<int,8> corners_orientation;

    static constexpr int adjacent_faces[6][4] = {
      {L_FACE, B_FACE, R_FACE, F_FACE},  // U (Up)
      {U_FACE, F_FACE, D_FACE, B_FACE},  // L (Left)
      {U_FACE, R_FACE, D_FACE, L_FACE},  // F (Front)
      {U_FACE, B_FACE, D_FACE, F_FACE},  // R (Right)
      {U_FACE, L_FACE, D_FACE, R_FACE},  // B (Back)
      {L_FACE, F_FACE, R_FACE, B_FACE}   // D (Down)
    };

    static constexpr int adjacent_outer_edges[6][4][4] = {
        {{0,1,2,3}, {0,1,2,3}, {0,1,2,3}, {0,1,2,3}},                // U (Up)
        {{0,4,8,12}, {0,4,8,12}, {0,4,8,12}, {15,11,7,3}},           // L (Left)
        {{12,13,14,15}, {0,4,8,12}, {3,2,1,0}, {15,11,7,3}},         // F (Front)
        {{3,7,11,15}, {12,8,4,0}, {3,7,11,15}, {3,7,11,15}},         // R (Right)
        {{0,1,2,3}, {12,8,4,0}, {15,14,13,12}, {3,7,11,15}},         // B (Back)
        {{12,13,14,15}, {12,13,14,15}, {12,13,14,15}, {12,13,14,15}} // D (Down)
    };

    static constexpr int adjacent_inner_edges[6][4][4] = {
        {{4,5,6,7}, {4,5,6,7}, {4,5,6,7}, {4,5,6,7}},        // U (Up)
        {{1,5,9,13}, {1,5,9,13}, {1,5,9,13}, {14,10,6,2}},   // L (Left)
        {{8,9,10,11}, {1,5,9,13}, {7,6,5,4}, {14,10,6,2}},   // F (Front)
        {{2,6,10,14}, {13,9,5,1}, {2,6,10,14}, {2,6,10,14}}, // R (Right)
        {{4,5,6,7}, {13,9,5,1}, {11,10,9,8}, {2,6,10,14}},   // B (Back)
        {{8,9,10,11}, {8,9,10,11}, {8,9,10,11}, {8,9,10,11}} // D (Down)
    };

    static constexpr int edge_pairs_swaps[6][4] = {
        {0, 1, 2, 3},     // U_FACE  
        {3, 11, 7, 10},   // L_FACE 
        {2, 8, 4, 11},    // F_FACE 
        {1, 9, 5, 8},     // R_FACE 
        {0, 10, 6, 9},    // B_FACE 
        {4, 5, 6, 7},     // D_FACE  
    };

    static constexpr int corner_swaps[6][4] = {
        {0, 1, 2, 3},     // U_FACE  
        {0, 3, 4, 7},   // L_FACE 
        {3, 2, 5, 4},    // F_FACE 
        {2, 1, 6, 5},     // R_FACE 
        {0, 7, 6, 1},    // B_FACE 
        {4, 5, 6, 7},     // D_FACE  
    };
    
    void turn_outer_face(int face_index, bool clockwise);
    void turn_inner_slice(int face_index, bool clockwise);

    static char colour_to_char(Colour c);
    static Colour char_to_colour(char c);
    static std::string char_to_emoji(char c);
};

std::string move_to_string(Move move);

#endif  // CUBE_H
