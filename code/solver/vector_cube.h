#ifndef VECTOR_CUBE_H
#define VECTOR_CUBE_H

#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>

class RubiksCube4x4 {
private:
    std::vector<char> facelets;

public:
    RubiksCube4x4();

    void display_cube();

    char get_index_face(int index);
    std::vector<int> find_centre_pieces(char colour);
    std::vector<int> find_edge_pieces(char colour);
    std::vector<int> find_corner_pieces(char colour);
    std::vector<std::vector<std::vector<std::vector<int>>>> find_unpaired_edges();
    std::vector<int> find_matching_edge_piece(std::vector<int> edge);

    std::pair<std::vector<int>, std::vector<int>> find_spots_in_centre(char face, char colour);
    std::string rotate_cube_so_piece_on_face(int index, char face);

    void apply_random_moves(bool pause);
    void apply_moves(const std::vector<std::string> moves);
    std::vector<std::string>  apply_opposite_moves(const std::vector<std::string> moves);
    std::string apply_opposite_move(const std::string move);
    void apply_move(const std::string move);
    void apply_index_swaps(const std::unordered_map<int, int> index_move_map, bool clockwise);
    bool verify_valid_cube();

    void rotate_x(bool clockwise);
    void rotate_y(bool clockwise);
    void rotate_z(bool clockwise);

    void move_R(bool clockwise);
    void move_r(bool clockwise);
    void move_Rw(bool clockwise);
    void move_L(bool clockwise);
    void move_l(bool clockwise);
    void move_Lw(bool clockwise);
    void move_U(bool clockwise);
    void move_u(bool clockwise);
    void move_Uw(bool clockwise);
    void move_D(bool clockwise);
    void move_d(bool clockwise);
    void move_Dw(bool clockwise);
    void move_F(bool clockwise);
    void move_f(bool clockwise);
    void move_Fw(bool clockwise);
    void move_B(bool clockwise);
    void move_b(bool clockwise);
    void move_Bw(bool clockwise);
};

#endif