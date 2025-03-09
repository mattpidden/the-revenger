#ifndef CUBE_H
#define CUBE_H

#include <array>
#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>
#include <functional>
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
    std::array<std::array<int, 16>, 6> facelets;
 
    void reset();  
    std::string export_state() const;  
    void print() const;  
    void move(Move m); 
    std::vector<Move> apply_random_moves(int n,  std::vector<Move> all_moves);
    bool check_solved() const;
    void apply_mask(const std::vector<int>& mask);
    void apply_colour_mask(int value, const std::vector<int>& mask);
    void apply_location_colour_mask(int value, const std::vector<int>& location_mask);
    
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

    static char id_to_char(int id);
    static int char_to_id(char c);
    static std::string char_to_emoji(char c);
private:

    
};

std::string move_to_string(Move move);

class Phase {
public:
    std::string name;
    std::vector<Move> moves;
    std::function<bool(const Cube4x4&)> is_solved;
    std::function<std::string(Cube4x4&)> mask;
    std::string table_filename;
    int table_depth_limit;
    int search_depth_limit;
    std::unordered_map<std::string, int> table;

    Phase(const std::string& name, const std::vector<Move>& moves, std::function<bool(const Cube4x4&)> is_solved, std::function<std::string(Cube4x4&)> mask, const std::string& table_filename, int table_depth_limit, int search_depth_limit) : name(name), moves(moves), is_solved(is_solved), mask(mask), table_filename(table_filename), table_depth_limit(table_depth_limit), search_depth_limit(search_depth_limit) {}

    void set_table(const std::unordered_map<std::string, int>& new_table) {
        table = new_table;
    }
};

extern Phase phase1;
extern Phase phase2;
extern Phase phase3;
extern Phase phase4;
extern Phase phase5;
extern Phase phase6;
extern Phase phase7;
extern Phase phase8;

#endif  // CUBE_H
