#include "vector_cube.h"

int main() {
    RubiksCube4x4 cube;

    cube.apply_random_moves();
    cube.display_cube();

    return 0;
}
