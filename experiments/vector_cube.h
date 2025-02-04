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
    std::vector<std::string> generate_random_moves(int num);
    void apply_moves(const std::vector<std::string> moves);
    void apply_move(const std::string move);
    void apply_index_swaps(const std::unordered_map<int, int> index_move_map, bool clockwise);

    void move_R(bool clockwise);
    void move_L(bool clockwise);
    void move_U(bool clockwise);
    void move_D(bool clockwise);
    void move_F(bool clockwise);
    void move_B(bool clockwise);
};

#endif