#ifndef REDUCE_CENTRES_H
#define REDUCE_CENTRES_H

#include "vector_cube.h"

std::vector<std::string> create_white_centre(RubiksCube4x4& cube);
std::vector<std::string> create_any_centre(RubiksCube4x4& cube, char colour);

#endif