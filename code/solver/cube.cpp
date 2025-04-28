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

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


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
    result.reserve(96);
    auto appendFace = [&](Face f) {
        for(int i=0; i<16; i++){
            result.push_back(id_to_char(facelets[f][i]) );
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

void Cube4x4::valid_facelets_ids() {
    std::unordered_set<int> seen_ids;
    for (int face = 0; face < 6; ++face) {
        for (int pos = 0; pos < 16; ++pos) {
            int id = facelets[face][pos];
            if (id < 0 || id > 95) {
                throw std::invalid_argument("Error: Invalid id not in range");
            }
            if (seen_ids.count(id) > 0) { 
                throw std::invalid_argument("Error: Duplicate id in facelets");
            }
            seen_ids.insert(id);
        }
    }
    if (seen_ids.size() != 96) {
        throw std::invalid_argument("Error: Incorrect total number of ids");
    }
}

void Cube4x4::check_state_is_valid(const std::string& input_string) {
    if (input_string.size() != 96) {
        throw std::invalid_argument("Error: The input string must be of length 96 chars. (found " + std::to_string(input_string.size()) + ")");
    }
    std::unordered_map<char, int> color_count = { {'W', 0}, {'O', 0}, {'G', 0}, {'R', 0}, {'B', 0}, {'Y', 0} };
    for (char c : input_string) {
        if (color_count.find(c) == color_count.end()) {
            throw std::invalid_argument("Error: The input string contained invalid char. (found '" + std::string(1, c) + ")");
        }
        color_count[c]++;
    }
    for (const auto& [color, count] : color_count) {
        if (count != 16) {
            throw std::invalid_argument("Error: The input string must contain exactly 16 '" + std::string(1, color) + "' facelets. (found " + std::to_string(count) + ")");
        }
    }
    std::unordered_map<char, int> centre_colour_count = { {'W', 0}, {'O', 0}, {'G', 0}, {'R', 0}, {'B', 0}, {'Y', 0} };
    static const std::vector<int> centre_indices = {5,6,9,10,85,86,89,90,37,38,41,42,69,70,73,74,21,22,25,26,53,54,57,58};
    for (int i : centre_indices) {
        centre_colour_count[input_string[i]]++;
    }
    for (const auto& [color, count] : centre_colour_count) {
        if (count != 4) {
            throw std::invalid_argument("Error: The input string must contain exactly 4 '" + std::string(1, color) + "' centre facelets. (found " + std::to_string(count) + ")");
        }
    }
    std::unordered_map<char, int> edge_colour_count = { {'W', 0}, {'O', 0}, {'G', 0}, {'R', 0}, {'B', 0}, {'Y', 0} };
    static const std::vector<int> edge_indices = {1,2,4,8,7,11,13,14,17,18,33,34,49,50,65,66,29,30,45,46,61,62,77,78,81,82,84,88,87,91,93,94,20,24,23,27,36,40,39,43,52,56,55,59,68,72,71,75};
    for (int i : edge_indices) {
        edge_colour_count[input_string[i]]++;
    }
    for (const auto& [color, count] : edge_colour_count) {
        if (count != 8) {
            throw std::invalid_argument("Error: The input string must contain exactly 8 '" + std::string(1, color) + "' edge facelets. (found " + std::to_string(count) + ")");
        }
    }
    std::unordered_map<char, int> corner_colour_count = { {'W', 0}, {'O', 0}, {'G', 0}, {'R', 0}, {'B', 0}, {'Y', 0} };
    static const std::vector<int> corner_indices = {0,3,12,15,16,19,28,31,32,35,44,47,48,51,60,63,64,67,76,79,80,83,92,95};
    for (int i : corner_indices) {
        corner_colour_count[input_string[i]]++;
    }
    for (const auto& [color, count] : corner_colour_count) {
        if (count != 4) {
            throw std::invalid_argument("Error: The input string must contain exactly 4 '" + std::string(1, color) + "' corner facelets. (found " + std::to_string(count) + ")");
        }
    }
}

bool Cube4x4::is_centre_facelet(int index) {
    static const std::unordered_set<int> centre_indices = {5,6,9,10,85,86,89,90,37,38,41,42,69,70,73,74,21,22,25,26,53,54,57,58};
    return centre_indices.count(index) > 0;
}

bool Cube4x4::is_edge_facelet(int index) {
    static const std::unordered_set<int> edge_indices = {14,94,8,1,7,88,81,87,56,55,23,24, 33,78,17,65,49,30,46,62,39,72,40,71}; 
    return edge_indices.count(index) > 0;
}

int get_matching_edge_facelet(int index) {
    static const std::unordered_map<int, int> edge_indices_pairs = {
        {1, 66}, {66, 1}, {2, 65}, {65, 2}, {4, 17}, {17, 4}, {8, 18}, {18, 8},
        {13, 33}, {33, 13}, {14, 34}, {34, 14}, {7, 50}, {50, 7}, {11, 49}, {49, 11},
        {36, 23}, {23, 36}, {40, 27}, {27, 40}, {39, 52}, {52, 39}, {43, 56}, {56, 43},
        {68, 55}, {55, 68}, {72, 59}, {59, 72}, {71, 20}, {20, 71}, {75, 24}, {24, 75},
        {93, 78}, {78, 93}, {94, 77}, {77, 94}, {84, 30}, {30, 84}, {88, 29}, {29, 88},
        {81, 45}, {45, 81}, {82, 46}, {46, 82}, {87, 61}, {61, 87}, {91, 62}, {62, 91}
    };
    auto it = edge_indices_pairs.find(index);
    if (it == edge_indices_pairs.end()) {
        return -1;
    }
    return it->second;
}

bool Cube4x4::is_corner_facelet(int index) {
    static const std::unordered_set<int> corner_indices = {0,3,12,15,16,19,28,31,32,35,44,47,48,51,60,63,64,67,76,79,80,83,92,95}; 
    return corner_indices.count(index) > 0;
}

std::array<int, 2> get_matching_corner_facelets(int facelet) { 
    std::array<std::array<int, 3>, 8> corner_facelets = {{
        {0, 16, 67}, {3, 51, 64}, {12, 19, 32}, {15, 35, 48},
        {31, 44, 80}, {47, 60, 83}, {28, 79, 92}, {63, 76, 95}
    }};
    for (const auto& corner : corner_facelets) {
        if (std::find(corner.begin(), corner.end(), facelet) != corner.end()) {
            std::array<int, 2> result;
            int index = 0;
            for (int f : corner) {
                if (f != facelet) {
                    result[index++] = f;
                }
            }
            return result;
        }
    }
    return {-1,-1};
}


void Cube4x4::import_state(const std::string& input_string) {
    std::unordered_map<char, std::unordered_set<int>> centre_colour_to_id = {{'W', {5,6,9,10}}, {'O', {21,22,25,26}}, {'G', {37,38,41,42}}, {'R', {53,54,57,58}}, {'B', {69,70,73,74}}, {'Y', {85,86,89,90}}};
    std::unordered_map<std::string, std::vector<int>> edge_id_to_id = {
        {"WB", {1,66}}, {"BW", {65,2}}, 
        {"WO", {8,18}}, {"OW", {17,4}}, 
        {"WG", {14,34}}, {"GW", {33,13}}, 
        {"WR", {7,50}}, {"RW", {49,11}}, 
        {"GO", {40,27}}, {"OG", {23,36}}, 
        {"GR", {39,52}}, {"RG", {56,43}}, 
        {"BR", {72,59}}, {"RB", {55,68}}, 
        {"BO", {71,20}}, {"OB", {24,75}}, 
        {"YB", {94,77}}, {"BY", {78,93}}, 
        {"YO", {88,29}}, {"OY", {30,84}}, 
        {"YG", {81,45}}, {"GY", {46,82}}, 
        {"YR", {87,61}}, {"RY", {62,91}}, 
    };
    std::unordered_map<std::string, int> corner_id_to_id = {
        {"WOB", 0},{"WBO", 0},{"OWB", 16},{"OBW", 16},{"BOW", 67},{"BWO", 67},
        {"WRB", 3},{"WBR", 3},{"RWB", 51},{"RBW", 51},{"BRW", 64},{"BWR", 64},
        {"WOG", 12},{"WGO", 12},{"OGW", 19},{"OWG", 19},{"GWO", 32},{"GOW", 32},
        {"WRG", 15},{"WGR", 15},{"GWR", 35},{"GRW", 35},{"RGW", 48},{"RWG", 48},
        {"YOB", 92},{"YBO", 92},{"OYB", 28},{"OBY", 28},{"BOY", 79},{"BYO", 79},
        {"YRB", 95},{"YBR", 95},{"RYB", 63},{"RBY", 63},{"BRY", 76},{"BYR", 76},
        {"YOG", 80},{"YGO", 80},{"OGY", 31},{"OYG", 31},{"GYO", 44},{"GOY", 44},
        {"YRG", 83},{"YGR", 83},{"GYR", 47},{"GRY", 47},{"RGY", 60},{"RYG", 60},
    };

    for (int face = 0; face < 6; ++face) {
        for (int pos = 0; pos < 16; ++pos) {
            int index = face * 16 + pos;
            char colour_char = input_string[index];
            if (is_centre_facelet(index)) {
                auto& indices = centre_colour_to_id[colour_char];
                int id = *indices.begin();
                indices.erase(indices.begin());  
                facelets[face][pos] = id;        
            }
            if (is_edge_facelet(index)) {
                int matching_index = get_matching_edge_facelet(index);
                std::string edge_key = std::string(1, colour_char) + input_string[matching_index];
                std::vector<int> indices = edge_id_to_id[edge_key];
                facelets[face][pos] = indices[0];
                facelets[matching_index / 16][matching_index % 16] = indices[1];
            }
            if (is_corner_facelet(index)) {
                std::array<int, 2> matching_indicies = get_matching_corner_facelets(index);  
                std::string edge_key = std::string(1, colour_char) + input_string[matching_indicies[0]] + input_string[matching_indicies[1]];
                int id = corner_id_to_id[edge_key];
                facelets[face][pos] = id;
            }
        }
    }
}


// Prints visulisation of cube in command line
void Cube4x4::print() const {
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
}


void Cube4x4::export_flat_image(const std::string& filename) const {
    std::string s = export_state(); 
    const int square_size = 40;
    const int img_width = square_size * 18;
    const int img_height = square_size * 14;

    std::vector<unsigned char> image(img_width * img_height * 3, 255);  // white background

    auto draw_square = [&](int x, int y, char colour) {
        int base_x = x * square_size;
        int base_y = y * square_size;
        uint8_t r = 0, g = 0, b = 0;
    
        switch (colour) {
            case 'W':  r = g = b = 255; break;
            case 'R':  r = 235; g = 48; b = 49; break;
            case 'O':  r = 232; g = 159; b = 21; break;
            case 'B':  r = 28; g = 93; b = 254; break;
            case 'G':  r = 87; g = 213; b = 105; break;
            case 'Y':  r = 242; g = 242; b = 20; break;
            default: break;
        }
    
        for (int dy = 0; dy < square_size; dy++) {
            for (int dx = 0; dx < square_size; dx++) {
                int px = base_x + dx;
                int py = base_y + dy;
                int idx = (py * img_width + px) * 3;
    
                // Draw black border (1px)
                if (dx == 0 || dx == square_size - 1 || dy == 0 || dy == square_size - 1) {
                    image[idx] = 0;
                    image[idx + 1] = 0;
                    image[idx + 2] = 0;
                } else {
                    image[idx] = r;
                    image[idx + 1] = g;
                    image[idx + 2] = b;
                }
            }
        }
    };
    

    // Offsets for each face in the flat layout
    std::map<int, std::pair<int, int>> face_offsets = {
        {0,  {5, 1}},  // U
        {1,  {1, 5}},  // L
        {2,  {5, 5}},  // F
        {3,  {9, 5}},  // R
        {4,  {13, 5}}, // B (note: requires img_width = 16 faces wide)
        {5,  {5, 9}},  // D
    };

    for (int face = 0; face < 6; face++) {
        auto [ox, oy] = face_offsets[face];
        for (int i = 0; i < 16; ++i) {
            int row = i / 4;
            int col = i % 4;
            char c = s[face * 16 + i];
            draw_square(ox + col, oy + row, c);
        }
    }

    stbi_write_png(filename.c_str(), img_width, img_height, 3, image.data(), img_width * 3);
    std::cout << "Cube image saved to " << filename << "\n";
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
        case Uw:  turn_outer_face(U_FACE, true); turn_inner_slice(U_FACE, true); break;
        case Uw_PRIME: turn_outer_face(U_FACE, false); turn_inner_slice(U_FACE, false); break;
        case Uw2: turn_outer_face(U_FACE, true); turn_outer_face(U_FACE, true); turn_inner_slice(U_FACE, true); turn_inner_slice(U_FACE, true); break;
        case Lw:  turn_outer_face(L_FACE, true); turn_inner_slice(L_FACE, true);  break;
        case Lw_PRIME: turn_outer_face(L_FACE, false); turn_inner_slice(L_FACE, false);  break;
        case Lw2: turn_outer_face(L_FACE, true); turn_outer_face(L_FACE, true); turn_inner_slice(L_FACE, true); turn_inner_slice(L_FACE, true); break;
        case Fw:  turn_outer_face(F_FACE, true); turn_inner_slice(F_FACE, true); break;
        case Fw_PRIME: turn_outer_face(F_FACE, false); turn_inner_slice(F_FACE, false); break;
        case Fw2: turn_outer_face(F_FACE, true); turn_outer_face(F_FACE, true); turn_inner_slice(F_FACE, true); turn_inner_slice(F_FACE, true); break;
        case Rw:  turn_outer_face(R_FACE, true); turn_inner_slice(R_FACE, true); break;
        case Rw_PRIME: turn_outer_face(R_FACE, false); turn_inner_slice(R_FACE, false); break;
        case Rw2: turn_outer_face(R_FACE, true); turn_outer_face(R_FACE, true); turn_inner_slice(R_FACE, true); turn_inner_slice(R_FACE, true); break;
        case Bw:  turn_outer_face(B_FACE, true); turn_inner_slice(B_FACE, true); break;
        case Bw_PRIME: turn_outer_face(B_FACE, false); turn_inner_slice(B_FACE, false); break;
        case Bw2: turn_outer_face(B_FACE, true); turn_outer_face(B_FACE, true); turn_inner_slice(B_FACE, true); turn_inner_slice(B_FACE, true); break;
        case Dw:  turn_outer_face(D_FACE, true); turn_inner_slice(D_FACE, true); break;
        case Dw_PRIME: turn_outer_face(D_FACE, false); turn_inner_slice(D_FACE, false); break;
        case Dw2: turn_outer_face(D_FACE, true); turn_outer_face(D_FACE, true); turn_inner_slice(D_FACE, true); turn_inner_slice(D_FACE, true); break;

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
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 16; j++) {
            if (std::find(mask.begin(), mask.end(), facelets[i][j]) == mask.end()) {
                if (facelets[i][j] >= 0) {
                    facelets[i][j] = -1;
                }
            }
        }
    }
}

void Cube4x4::apply_colour_mask(int value, const std::vector<int>& mask) {
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 16; j++) {
            if (std::find(mask.begin(), mask.end(), facelets[i][j]) != mask.end()) {
                facelets[i][j] = value;
            }
        }
    }
}

void Cube4x4::apply_location_colour_mask(int value, const std::vector<int>& location_mask) {
    for (int location : location_mask) {
        facelets[location / 16][location % 16] = value;
    }
}

bool Cube4x4::edge_piece_permutation_parity() {
    std::vector<int> low_edges_values = {facelets[U_FACE][1], facelets[U_FACE][7], facelets[U_FACE][8], facelets[U_FACE][14], facelets[L_FACE][7], facelets[L_FACE][8], facelets[R_FACE][7], facelets[R_FACE][8], 
        facelets[D_FACE][1], facelets[D_FACE][7], facelets[D_FACE][8], facelets[D_FACE][14]};
    std::vector<int> high_edges_values = {facelets[U_FACE][2], facelets[U_FACE][4], facelets[U_FACE][11], facelets[U_FACE][13], facelets[L_FACE][4], facelets[L_FACE][11], facelets[R_FACE][4], facelets[R_FACE][11], 
        facelets[D_FACE][2], facelets[D_FACE][4], facelets[D_FACE][11], facelets[D_FACE][13]};
    int low_inversions = 0;
    for (size_t i = 0; i < low_edges_values.size(); ++i) {
        for (size_t j = i + 1; j < low_edges_values.size(); ++j) {
            if (low_edges_values[i] > low_edges_values[j]) {
                low_inversions++;
            }
        }
    }
    bool low_edge_parity = (low_inversions % 2 == 0);
    int high_inversions = 0;
    for (size_t i = 0; i < high_edges_values.size(); ++i) {
        for (size_t j = i + 1; j < high_edges_values.size(); ++j) {
            if (high_edges_values[i] > high_edges_values[j]) {
                high_inversions++;
            }
        }
    }
    bool high_edge_parity = (high_inversions % 2 == 0);
    return (low_edge_parity == high_edge_parity);
}

int get_edge_cubelet_id(int facelet1, int facelet2, int facelet3, int facelet4) {
    std::array<int, 4> input = {facelet1, facelet2,facelet3, facelet4};
    std::sort(input.begin(), input.end());
    if (input == std::array<int,4>{1, 2, 65, 66}) { return 0; }             // UB
    else if (input == std::array<int,4>{4, 8, 17, 18}) { return 2; }        // UL
    else if (input == std::array<int,4>{7, 11, 49, 50}) { return 4; }       // UR
    else if (input == std::array<int,4>{13, 14, 33, 34}) { return 6; }      // UF
    else if (input == std::array<int,4>{23, 27, 36, 40}) { return 8; }      // FL
    else if (input == std::array<int,4>{39, 43, 52, 56}) { return 10; }     // FR
    else if (input == std::array<int,4>{55, 59, 68, 72}) { return 12; }     // RB
    else if (input == std::array<int,4>{20, 24, 71, 75}) { return 14; }     // BL
    else if (input == std::array<int,4>{45, 46, 81, 82}) { return 16; }     // DF
    else if (input == std::array<int,4>{29, 30, 84, 88}) { return 18; }     // DL
    else if (input == std::array<int,4>{61, 62, 87, 91}) { return 20; }     // DR
    else if (input == std::array<int,4>{77, 78, 93, 94}) { return 22; }     // DB
    else { return -1; }
}

int get_corner_cubelet_id(int facelet1, int facelet2, int facelet3) {  
    std::array<int, 3> input = {facelet1, facelet2, facelet3};
    std::sort(input.begin(), input.end());
    if (input == std::array<int,3>{0, 16, 67}) { return 0; } 
    else if (input == std::array<int,3>{3, 51, 64}) { return 1; } 
    else if (input == std::array<int,3>{12, 19, 32}) { return 2; } 
    else if (input == std::array<int,3>{15, 35, 48}) { return 3; } 
    else if (input == std::array<int,3>{31, 44, 80}) { return 4; } 
    else if (input == std::array<int,3>{47, 60, 83}) { return 5; } 
    else if (input == std::array<int,3>{28, 79, 92}) { return 6; } 
    else if (input == std::array<int,3>{63, 76, 95}) { return 7; }
    else { return -1; }
}

//                  0  1  2  3
//                  4  5  6  7
//                  8  9  10 11
//                  12 13 14 15
//
//  16 17 18 19     32 33 34 35     48 49 50 51     64 65 66 67
//  20 21 22 23     36 37 38 39     52 53 54 55     68 69 70 71
//  24 25 26 27     40 41 42 43     56 57 58 59     72 73 74 75
//  28 29 30 31     44 45 46 47     60 61 62 63     76 77 78 79
//
//                  80 81 82 83     
//                  84 85 86 87
//                  88 89 90 91
//                  92 93 94 95


//                  0  1  2  3
//                  4  5  6  7
//                  8  9  10 11
//                  12 13 14 15
//
//  0  1  2  3      0  1  2  3      0  1  2  3      0  1  2  3 
//  4  5  6  7      4  5  6  7      4  5  6  7      4  5  6  7
//  8  9  10 11     8  9  10 11     8  9  10 11     8  9  10 11
//  12 13 14 15     12 13 14 15     12 13 14 15     12 13 14 15
//
//                  0  1  2  3      
//                  4  5  6  7
//                  8  9  10 11
//                  12 13 14 15

bool Cube4x4::edge_corner_permutation_parity() {

    std::vector<int> edge_values = {
        get_edge_cubelet_id(facelets[U_FACE][1], facelets[B_FACE][2], facelets[U_FACE][2], facelets[B_FACE][1]),
        get_edge_cubelet_id(facelets[U_FACE][4], facelets[L_FACE][1], facelets[U_FACE][8], facelets[L_FACE][2]),
        get_edge_cubelet_id(facelets[U_FACE][7], facelets[R_FACE][2], facelets[U_FACE][11], facelets[R_FACE][1]),
        get_edge_cubelet_id(facelets[U_FACE][13], facelets[F_FACE][1], facelets[U_FACE][14], facelets[F_FACE][2]),
        get_edge_cubelet_id(facelets[L_FACE][7], facelets[F_FACE][4], facelets[L_FACE][11], facelets[F_FACE][8]),
        get_edge_cubelet_id(facelets[F_FACE][7], facelets[R_FACE][4], facelets[F_FACE][11], facelets[R_FACE][8]),
        get_edge_cubelet_id(facelets[R_FACE][7], facelets[B_FACE][4], facelets[R_FACE][11], facelets[B_FACE][8]),
        get_edge_cubelet_id(facelets[B_FACE][7], facelets[L_FACE][4], facelets[B_FACE][11], facelets[L_FACE][8]),
        get_edge_cubelet_id(facelets[D_FACE][1], facelets[F_FACE][13], facelets[D_FACE][2], facelets[F_FACE][14]),
        get_edge_cubelet_id(facelets[D_FACE][4], facelets[L_FACE][14], facelets[D_FACE][8], facelets[L_FACE][13]),
        get_edge_cubelet_id(facelets[D_FACE][7], facelets[R_FACE][13], facelets[D_FACE][11], facelets[R_FACE][14]),
        get_edge_cubelet_id(facelets[D_FACE][13], facelets[B_FACE][14], facelets[D_FACE][14], facelets[B_FACE][13]),
    }; 

    std::vector<int> corner_values = {
        get_corner_cubelet_id(facelets[U_FACE][0], facelets[L_FACE][0], facelets[B_FACE][3]), 
        get_corner_cubelet_id(facelets[U_FACE][3], facelets[R_FACE][3], facelets[B_FACE][0]),
        get_corner_cubelet_id(facelets[U_FACE][12], facelets[L_FACE][3], facelets[F_FACE][0]),
        get_corner_cubelet_id(facelets[U_FACE][15], facelets[F_FACE][3], facelets[R_FACE][0]),
        get_corner_cubelet_id(facelets[D_FACE][0], facelets[L_FACE][15], facelets[F_FACE][12]),
        get_corner_cubelet_id(facelets[D_FACE][3], facelets[F_FACE][15], facelets[R_FACE][12]),
        get_corner_cubelet_id(facelets[D_FACE][12], facelets[L_FACE][12], facelets[B_FACE][15]),
        get_corner_cubelet_id(facelets[D_FACE][15], facelets[R_FACE][15], facelets[B_FACE][12])
    };
    
    int edge_inversions = 0;
    for (size_t i = 0; i < edge_values.size(); ++i) {
        for (size_t j = i + 1; j < edge_values.size(); ++j) {
            if (edge_values[i] > edge_values[j]) {
                edge_inversions++;
            }
        }
    }
    bool edge_parity = (edge_inversions % 2 == 0);
    int corner_inversions = 0;
    for (size_t i = 0; i < corner_values.size(); ++i) {
        for (size_t j = i + 1; j < corner_values.size(); ++j) {
            if (corner_values[i] > corner_values[j]) {
                corner_inversions++;
            }
        }
    }
    bool corner_parity = (corner_inversions % 2 == 0);
    return (edge_parity == corner_parity);
}



// Converts a facelet id to a char
char Cube4x4::id_to_char(int id) {
    if (id >= 0) {
        return "WOGRBY"[id / 16];
    }
    switch (id) {
        case -1: return 'X';
        case -2: return '2';
        case -3: return '3';
        case -4: return '4';
        case -5: return '5';
        case -6: return '6';
        case -7: return '7';
        case -8: return '8';
        case -9: return '9';
        case -10: return 'A';
        case -11: return 'C';
        case -12: return 'D';
        case -13: return 'E';
        case -14: return 'F';
        case -15: return 'H';
        default: 
            return 'Z';
    }
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
        case '6': return -6;
        case '7': return -7;
        case '8': return -8;
        case '9': return -9;
        case 'A': return -10;
        case 'C': return -11;
        case 'D': return -12;
        case 'E': return -13;
        case 'F': return -14;
        case 'H': return -15;
        default:
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
        case '2':  return "🟫";
        case '3':  return "🟪";
        default:   return std::string(2, c); 
    }
}

Move Cube4x4::string_to_move(const std::string& moveStr) {
    static const std::unordered_map<std::string, Move> moveMap = {
        {"R", R}, {"R'", R_PRIME}, {"R2", R2},
        {"L", L}, {"L'", L_PRIME}, {"L2", L2},
        {"U", U}, {"U'", U_PRIME}, {"U2", U2},
        {"D", D}, {"D'", D_PRIME}, {"D2", D2},
        {"F", F}, {"F'", F_PRIME}, {"F2", F2},
        {"B", B}, {"B'", B_PRIME}, {"B2", B2},
        {"r", r}, {"r'", r_PRIME}, {"r2", r2},
        {"l", l}, {"l'", l_PRIME}, {"l2", l2},
        {"u", u}, {"u'", u_PRIME}, {"u2", u2},
        {"d", d}, {"d'", d_PRIME}, {"d2", d2},
        {"f", f}, {"f'", f_PRIME}, {"f2", f2},
        {"b", b}, {"b'", b_PRIME}, {"b2", b2},
        {"Rw", Rw}, {"Rw'", Rw_PRIME}, {"Rw2", Rw2},
        {"Lw", Lw}, {"Lw'", Lw_PRIME}, {"Lw2", Lw2},
        {"Uw", Uw}, {"Uw'", Uw_PRIME}, {"Uw2", Uw2},
        {"Dw", Dw}, {"Dw'", Dw_PRIME}, {"Dw2", Dw2},
        {"Fw", Fw}, {"Fw'", Fw_PRIME}, {"Fw2", Fw2},
        {"Bw", Bw}, {"Bw'", Bw_PRIME}, {"Bw2", Bw2},
    };

    auto it = moveMap.find(moveStr);
    if (it != moveMap.end()) return it->second;
    throw std::invalid_argument("Invalid move string: " + moveStr);
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
        case Rw: return "Rw"; case Rw_PRIME: return "Rw'"; case Rw2: return "Rw2";
        case Lw: return "Lw"; case Lw_PRIME: return "Lw'"; case Lw2: return "Lw2";
        case Uw: return "Uw"; case Uw_PRIME: return "Uw'"; case Uw2: return "Uw2";
        case Dw: return "Dw"; case Dw_PRIME: return "Dw'"; case Dw2: return "Dw2";
        case Fw: return "Fw"; case Fw_PRIME: return "Fw'"; case Fw2: return "Fw2";
        case Bw: return "Bw"; case Bw_PRIME: return "Bw'"; case Bw2: return "Bw2";

        default: return "?";  
    }
}

int id(int face, int index) {
    return (face * 16) + index;
}

// PHASE 1 
std::vector<Move> phase1_moves = {R, R_PRIME, R2, L, L_PRIME, L2, F, F_PRIME, F2, B, B_PRIME, B2, U, U_PRIME, U2, D, D_PRIME, D2, r, r_PRIME, r2, l, l_PRIME, l2, f, f_PRIME, f2, b, b_PRIME, b2, u, u_PRIME, u2, d, d_PRIME, d2};
std::function<std::string(Cube4x4&)> phase1_mask = [](Cube4x4 cube) -> std::string {
    std::vector<int> mask = {id(L_FACE, NW_CENTRE), id(L_FACE, NE_CENTRE), id(L_FACE, SW_CENTRE), id(L_FACE, SE_CENTRE), id(R_FACE, NW_CENTRE), id(R_FACE, NE_CENTRE), id(R_FACE, SW_CENTRE), id(R_FACE, SE_CENTRE)};
    cube.apply_mask(mask);
    cube.apply_colour_mask(-2, mask);
    return cube.export_state();
};
Phase phase1("Phase 1", phase1_moves, phase1_mask, "phase1table", -1, 8, 8);

// PHASE 2a
std::vector<Move> phase2a_moves = {R, R_PRIME, R2, L, L_PRIME, L2, F, F_PRIME, F2, B, B_PRIME, B2, U, U_PRIME, U2, D, D_PRIME, D2, r, r_PRIME, r2, l, l_PRIME, l2, f2, b2, u2, d2};
std::function<std::string(Cube4x4&)> phase2a_mask = [](Cube4x4 cube) -> std::string {
    std::vector<int> mask = {37,38,41,42,69,70,73,74,5,6,9,10,85,86,89,90};
    std::vector<int> colour_mask1 = {5,6,9,10,85,86,89,90};
    std::vector<int> colour_mask2 = {37,38,41,42,69,70,73,74}; 
    cube.apply_mask(mask);
    cube.apply_colour_mask(0, colour_mask1);
    cube.apply_colour_mask(32, colour_mask2);
    return cube.export_state();
};
Phase phase2a("Phase 2a", phase2a_moves, phase2a_mask, "phase2atable", -1, 10, 1);


// PHASE 2b 
std::vector<Move> phase2b_moves = {R, R_PRIME, R2, L, L_PRIME, L2, F, F_PRIME, F2, B, B_PRIME, B2, U, U_PRIME, U2, D, D_PRIME, D2, r2, l2, f2, b2, u2, d2};
std::function<std::string(Cube4x4&)> phase2b_mask = [](Cube4x4 cube) -> std::string {
    std::vector<int> mask = {13,93,4,2,11,84,82,91,52,59,27,20,14,94,8,1,7,88,81,87,56,55,23,24,  21,22,25,26,53,54,57,58}; 
    std::vector<int> high_edges = {14,94,8,1,7,88,81,87,56,55,23,24}; 
    std::vector<int> low_edges = {13,93,4,2,11,84,82,91,52,59,27,20};
    cube.apply_mask(mask);
    cube.apply_colour_mask(-3, high_edges);
    cube.apply_colour_mask(-4, low_edges);
    return cube.export_state();
};
Phase phase2b("Phase 2b", phase2b_moves, phase2b_mask, "phase2btable", -1, 11, 16);

// PHASE 3 
std::vector<Move> phase3_moves = {R2, L2, F, F_PRIME, F2, B, B_PRIME, B2, U, U_PRIME, U2, D, D_PRIME, D2, r2, l2, f2, b2, u2, Dw2};
std::function<std::string(Cube4x4&)> phase3_mask = [](Cube4x4 cube) -> std::string {
    std::vector<int> mask = {37,38,41,42,69,70,73,74,21,22,25,26,53,54,57,58, 20,71, 24,75, 23,36, 27,40, 39,52, 43,56, 55,68, 59,72}; 
    std::vector<int> edge_facelets = {20,24,23,27,36,40,39,43,52,56,55,59,68,72,71,75}; 
    int solved_edges = -2;
    if (cube.id_to_char(cube.facelets[L_FACE][4]) == cube.id_to_char(cube.facelets[L_FACE][8])) {
        if (cube.id_to_char(cube.facelets[L_FACE][7]) == cube.id_to_char(cube.facelets[L_FACE][11])) {
            if (cube.id_to_char(cube.facelets[F_FACE][4]) == cube.id_to_char(cube.facelets[F_FACE][8])) {
                if (cube.id_to_char(cube.facelets[F_FACE][7]) == cube.id_to_char(cube.facelets[F_FACE][11])) {
                    if (cube.id_to_char(cube.facelets[R_FACE][4]) == cube.id_to_char(cube.facelets[R_FACE][8])) {
                        if (cube.id_to_char(cube.facelets[R_FACE][7]) == cube.id_to_char(cube.facelets[R_FACE][11])) {
                            if (cube.id_to_char(cube.facelets[B_FACE][4]) == cube.id_to_char(cube.facelets[B_FACE][8])) {
                                if (cube.id_to_char(cube.facelets[B_FACE][7]) == cube.id_to_char(cube.facelets[B_FACE][11])) {
                                    solved_edges = -3;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    cube.apply_mask(mask);
    cube.apply_colour_mask(solved_edges, edge_facelets);
    return cube.export_state();
};
Phase phase3("Phase 3", phase3_moves, phase3_mask, "phase3table", -1, 13, 16);

// PHASE 4
std::vector<Move> phase4_moves = {R2, L2, F2, B2, U, U_PRIME, U2, D, D_PRIME, D2, r2, l2, f2, b2};
std::function<std::string(Cube4x4&)> phase4_mask = [](Cube4x4 cube) -> std::string {
    std::vector<int> mask = {5,6,9,10,85,86,89,90,37,38,41,42,69,70,73,74,21,22,25,26,53,54,57,58}; // all centre pieces
    std::vector<int> edge_facelets = {1,2,4,8,7,11,13,14,17,18,33,34,49,50,65,66,29,30,45,46,61,62,77,78,81,82,84,88,87,91,93,94}; // all edge facelets except those in middle slice
    std::vector<int> paired_facelets = {};
    std::vector<int> unpaired_facelets = {};
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
    if (paired_facelets.size() == edge_facelets.size()) {
        bool parity = cube.edge_corner_permutation_parity();
        if (parity) { cube.apply_location_colour_mask(-5, {0}); };
    }
    cube.apply_mask(mask);
    cube.apply_location_colour_mask(-3, paired_facelets);
    cube.apply_location_colour_mask(-4, unpaired_facelets);

    return cube.export_state();
};
Phase phase4("Phase 4", phase4_moves, phase4_mask, "phase4table", -1, 25, 16);

// PHASE 6 
std::vector<Move> phase6_moves = {R, R_PRIME, R2, L, L_PRIME, L2, F2, B2, U, U_PRIME, U2, D, D_PRIME, D2};
std::function<std::string(Cube4x4&)> phase6_mask = [](Cube4x4 cube) -> std::string {
    std::vector<int> mask = {0,3,12,15,36,39,40,43,68,71,72,75,80,83,92,95};
    std::vector<int> orient_corners_mask = {0,3,12,15,80,83,92,95};
    std::vector<int> middle_slice_edges_mask = {36,39,40,43,68,71,72,75};
    cube.apply_mask(mask);
    cube.apply_colour_mask(-3, orient_corners_mask);
    cube.apply_colour_mask(-4, middle_slice_edges_mask);
    return cube.export_state();
};
Phase phase6("Phase 6", phase6_moves, phase6_mask, "phase6table", -1, 10, 4);

// PHASE 7 
std::vector<Move> phase7_moves = {R2, L2, F2, B2, U, U_PRIME, U2, D, D_PRIME, D2};
std::function<std::string(Cube4x4&)> phase7_mask = [](Cube4x4 cube) -> std::string {
    std::vector<int> mask = {16,19,28,31,32,35,44,47,48,51,60,63,64,67,76,79, 17,18,20,23,24,27,29,30,49,50,52,55,56,59,61,62,33,34,36,39,40,43,45,46,65,66,68,71,72,75,77,78};
    std::vector<int> edge_colour_mask1 = {17,18,20,23,24,27,29,30,49,50,52,55,56,59,61,62};
    std::vector<int> edge_colour_mask2 = {33,34,36,39,40,43,45,46,65,66,68,71,72,75,77,78};
    cube.apply_mask(mask);
    cube.apply_colour_mask(-5, edge_colour_mask1);
    cube.apply_colour_mask(-6, edge_colour_mask2);
    return cube.export_state();
};
Phase phase7("Phase 7", phase7_moves, phase7_mask, "phase7table", -1, 13, 4);

// PHASE 8
std::vector<Move> phase8_moves = {R2, L2, F2, B2, U2, D2};
std::function<std::string(Cube4x4&)> phase8_mask = [](Cube4x4 cube) -> std::string {
    std::vector<int> mask = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95};
    cube.apply_mask(mask);
    return cube.export_state();
};
Phase phase8("Phase 8", phase8_moves, phase8_mask, "phase8table", -1, 15, 4);
