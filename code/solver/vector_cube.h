#ifndef VECTOR_CUBE_H
#define VECTOR_CUBE_H

#include <vector>
#include <string>
#include <unordered_map>

class RubiksCube4x4 {
private:
    std::vector<char> facelets;

public:
    RubiksCube4x4();

    void display_cube();
    void apply_random_moves();
    void apply_moves(const std::string& moves);
    void apply_move(const std::string& move);
    void apply_index_swaps(const std::unordered_map<int, int>& index_move_map, bool clockwise);

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