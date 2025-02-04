#ifndef SIX_4X4_CUBE_H
#define SIX_4X4_CUBE_H

#include <vector>
#include <string>
#include <iostream>

class RubiksCube4x4 {
private:
    // Cube state: 6 faces, 4x4 grid per face
    char cube[6][4][4];
public:
    // Constructor
    RubiksCube4x4();

    // Cube manipulation methods
    void display_cube();
    void apply_move(const std::string move);
    void apply_moves(const std::vector<std::string> moves);

    // Move operations
    void move_R();
    void move_L();
    void move_U();
    void move_D();
    void move_F();
    void move_B();

    // Helper methods
    std::vector<std::string> generate_random_moves(int num);


};

#endif // SIX_4X4_CUBE_H
