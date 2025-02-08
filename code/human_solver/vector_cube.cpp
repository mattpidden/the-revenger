#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <sstream> 
#include <random>
#include <unordered_set>
#include <utility>

#include "vector_cube.h"

RubiksCube4x4::RubiksCube4x4() {
    facelets = {
        'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W',  // Up face  
        'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y',  // Down face
        'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B',  // Back face
        'G', 'G', 'G', 'G', 'G', 'G', 'G', 'G', 'G', 'G', 'G', 'G', 'G', 'G', 'G', 'G',  // Front face
        'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O',  // Left face
        'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R',  // Right face
    };
}

/*
             Up (U)
             0  1  2  3
             4  5  6  7
             8  9  10 11
             12 13 14 15

Left (L)     Front (F)    Right (R)    Back (B)
64 65 66 67  48 49 50 51  80 81 82 83  32 33 34 35
68 69 70 71  52 53 54 55  84 85 86 87  36 37 38 39
72 73 74 75  56 57 58 59  88 89 90 91  40 41 42 43
76 77 78 79  60 61 62 63  92 93 94 95  44 45 46 47

             Down (D)
             16 17 18 19
             20 21 22 23
             24 25 26 27
             28 29 30 31
*/


// Method to display the cube's facelets
void RubiksCube4x4::display_cube() {
    std::map<char, std::string> colours_to_emojis = {
        {'W', "⬜"}, {'Y', "🟨"}, {'G', "🟩"}, 
        {'B', "🟦"}, {'O', "🟧"}, {'R', "🟥"}
    };

    std::cout << "          Up (U)\n";
    for (int i = 0; i < 4; ++i) {
        std::cout << "          ";
        for (int j = 0; j < 4; ++j) {
            std::cout << colours_to_emojis[facelets[0 + i * 4 + j]];
        }
        std::cout << "\n";
    }
    std::cout << "\nLeft (L)  Front (F) Right (R) Back (B)\n";
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col)
            std::cout << colours_to_emojis[facelets[64 + row * 4 + col]];
        std::cout << "  ";
        for (int col = 0; col < 4; ++col)
            std::cout << colours_to_emojis[facelets[48 + row * 4 + col]];
        std::cout << "  ";
        for (int col = 0; col < 4; ++col)
            std::cout << colours_to_emojis[facelets[80 + row * 4 + col]];
        std::cout << "  ";
        for (int col = 0; col < 4; ++col)
            std::cout << colours_to_emojis[facelets[32 + row * 4 + col]];
        std::cout << "\n";
    }
    std::cout << "\n          Down (D)\n";
    for (int i = 0; i < 4; ++i) {
        std::cout << "          ";
        for (int j = 0; j < 4; ++j) {
            std::cout << colours_to_emojis[facelets[16 + i * 4 + j]];
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

char RubiksCube4x4::get_index_face(int index) {
    if (index >= 0 && index <= 15){
        return 'U';
    } else if (index >= 16 && index <= 31){
        return 'D';
    } else if (index >= 32 && index <= 47){
        return 'B';
    } else if (index >= 48 && index <= 63){
        return 'F';
    } else if (index >= 64 && index <= 79){
        return 'L';
    } else {
        return 'R';
    }
}

std::vector<int> RubiksCube4x4::find_centre_pieces(char colour) {
    std::vector<int> centres = {5,6,9,10,21,22,25,26,37,38,41,42,53,54,57,58,69,70,73,74,85,86,89,90};
    std::vector<int> matching_centres;
    for (int index : centres) {
        if (facelets[index] == colour) {
            matching_centres.push_back(index);
        }
    }

    return matching_centres;
}

std::vector<int> RubiksCube4x4::find_edge_pieces(char colour) {
    std::vector<int> edges = {1,2,4,7,8,11,13,14,17,18,20,23,24,27,29,30,33,34,36,39,40,43,45,46,49,50,52,55,56,59,61,62,65,66,68,71,72,75,77,78,81,82,84,87,88,91,93,94};
    std::vector<int> matching_edges;
    for (int index : edges) {
        if (facelets[index] == colour) {
            matching_edges.push_back(index);
        }
    }

    return matching_edges;
}

std::vector<int> RubiksCube4x4::find_corner_pieces(char colour) {
    std::vector<int> corners = {0,3,12,15,16,19,28,31,32,35,44,47,48,51,60,63,64,67,76,79,80,83,92,95};
    std::vector<int> matching_corners;
    for (int index : corners) {
        if (facelets[index] == colour) {
            matching_corners.push_back(index);
        }
    }

    return matching_corners;
}

std::vector<std::vector<std::vector<std::vector<int>>>> RubiksCube4x4::find_paired_edges() {
    std::vector<std::vector<std::vector<int>>> edge_pieces = {{{4,65}, {8,66}}, {{7,82}, {11,81}}, {{13,49}, {14,50}}, {{52,71}, {56,75}}, {{55,84}, {59,88}}, {{61,17}, {62,18}}, {{36,87}, {40,91}}, {{39,68}, {43,72}}, {{1,34}, {2,33}}, {{20,78}, {24,77}}, {{23,93}, {27,94}}, {{29,46}, {30,45}}};
    std::vector<std::vector<std::vector<int>>> unpaired_edges = {};
    std::vector<std::vector<std::vector<int>>> paired_edges = {};
    for (const auto& edge : edge_pieces) {
        if (facelets[edge[0][0]] != facelets[edge[1][0]] || facelets[edge[0][1]] != facelets[edge[1][1]]) {
            unpaired_edges.push_back(edge);
        } else {
            paired_edges.push_back(edge);
        }
    }
    return {paired_edges, unpaired_edges};
}

std::vector<int> RubiksCube4x4::find_matching_edge_piece(std::vector<int> single_edge) {
    std::vector<std::vector<std::vector<int>>> edge_pieces = {{{4,65}, {8,66}}, {{7,82}, {11,81}}, {{13,49}, {14,50}}, {{52,71}, {56,75}}, {{55,84}, {59,88}}, {{61,17}, {62,18}}, {{36,87}, {40,91}}, {{39,68}, {43,72}}, {{1,34}, {2,33}}, {{20,78}, {24,77}}, {{23,93}, {27,94}}, {{29,46}, {30,45}}};
    for (const auto& edge : edge_pieces) {
        if (((facelets[edge[0][0]] == facelets[single_edge[0]] && facelets[edge[0][1]] == facelets[single_edge[1]]) || (facelets[edge[0][0]] == facelets[single_edge[1]] && facelets[edge[0][1]] == facelets[single_edge[0]])) && single_edge != edge[0]) {
            return edge[0];
        }
        if (((facelets[edge[1][0]] == facelets[single_edge[0]] && facelets[edge[1][1]] == facelets[single_edge[1]]) || (facelets[edge[1][0]] == facelets[single_edge[1]] && facelets[edge[1][1]] == facelets[single_edge[0]])) && single_edge != edge[1]) {
            return edge[1];
        }
    }
    return {};
}



std::pair<std::vector<int>, std::vector<int>> RubiksCube4x4::find_spots_in_centre(char face, char colour) {
    std::map<char, std::vector<int>> faces_centres = {
        {'U', {5,6,9,10}}, {'D', {21,22,25,26}}, {'B', {37,38,41,42}}, 
        {'F', {53,54,57,58}}, {'L', {69,70,73,74}}, {'R', {85,86,89,90}}
    };
    std::vector<int> spots;
    std::vector<int> empty_spots;
    std::vector<int> centres = faces_centres[face];
    for (int index : centres) {
        if (facelets[index] == colour) {
            spots.push_back(index);
        } else {
            empty_spots.push_back(index);
        }
    }

    return {spots, empty_spots};
}

std::vector<std::string> RubiksCube4x4::rotate_cube_so_piece_on_face(int index, char face) {
    char piece_face = get_index_face(index);

    std::map<std::pair<char, char>, std::vector<std::string>> lookup = {
        {{'U', 'F'}, {"x"}},
        {{'U', 'B'}, {"x'"}},
        {{'U', 'D'}, {"x2"}},
        {{'U', 'R'}, {"z'"}},
        {{'U', 'L'}, {"z"}},
        {{'U', 'U'}, {}},
        {{'D', 'F'}, {"x'"}},
        {{'D', 'B'}, {"x"}},
        {{'D', 'D'}, {}},
        {{'D', 'R'}, {"z"}},
        {{'D', 'L'}, {"z'"}},
        {{'D', 'U'}, {"x2"}},
        {{'F', 'F'}, {}},
        {{'F', 'B'}, {"y2"}},
        {{'F', 'D'}, {"x"}},
        {{'F', 'R'}, {"y"}},
        {{'F', 'L'}, {"y'"}},
        {{'F', 'U'}, {"x'"}},
        {{'B', 'F'}, {"y2"}},
        {{'B', 'B'}, {}},
        {{'B', 'D'}, {"x'"}},
        {{'B', 'R'}, {"y'"}},
        {{'B', 'L'}, {"y"}},
        {{'B', 'U'}, {"x"}},
    };

    return lookup[{face, piece_face}];
}

bool RubiksCube4x4::check_indexes_match_colour(std::vector<int> indexes, char colour) {
    for (int i : indexes) {
        if (facelets[i] != colour) {
            return false;
        }
    }
    return true;
}

bool RubiksCube4x4::check_indexes_match_indexes(std::vector<std::vector<int>> indexes) {
    for (const std::vector<int>& i : indexes) {
        for (size_t j = 1; j < i.size(); ++j) { 
            if (facelets[i[j]] != facelets[i[0]]) {
                return false;
            }
        }
    }
    return true;
}


bool RubiksCube4x4::check_corners_aligned_pll() {
    if (facelets[64] == facelets[68] && facelets[48] == facelets[52] && facelets[80] == facelets[84] && facelets[32] == facelets[36]) {
        return true;
    } else {
        return false;
    }
}

std::vector<std::vector<std::vector<int>>> RubiksCube4x4::check_edge_centre_alignment() {
    std::vector<std::vector<int>> aligned_edges = {};
    std::vector<std::vector<int>> unaligned_edges = {};

    if (facelets[65] == facelets[66] && facelets[66] == facelets[69] &&
        facelets[69] == facelets[70] && facelets[70] == facelets[73] &&
        facelets[73] == facelets[74]) {
        aligned_edges.push_back({65,66});
    } else {
        unaligned_edges.push_back({65,66});
    }

    if (facelets[49] == facelets[50] && facelets[50] == facelets[53] &&
        facelets[53] == facelets[54] && facelets[54] == facelets[57] &&
        facelets[57] == facelets[58]) {
        aligned_edges.push_back({49,50});
    } else {
        unaligned_edges.push_back({49,50});
    }

    if (facelets[81] == facelets[82] && facelets[82] == facelets[85] &&
        facelets[85] == facelets[86] && facelets[86] == facelets[89] &&
        facelets[89] == facelets[90]) {
        aligned_edges.push_back({81,82});
    } else {
        unaligned_edges.push_back({81,82});
    }

    if (facelets[33] == facelets[34] && facelets[34] == facelets[37] &&
        facelets[37] == facelets[38] && facelets[38] == facelets[41] &&
        facelets[41] == facelets[42]) {
        aligned_edges.push_back({33,34});
    } else {
        unaligned_edges.push_back({33,34});
    }

    return {aligned_edges, unaligned_edges};
}


std::vector<std::string> RubiksCube4x4::decide_edge_flip() {
    if (facelets[52] != facelets[55] || facelets[56] != facelets[59]) {
        return {"R", "F'", "U", "R'", "F"};
    } else {
        return {};
    }
}

std::vector<int> RubiksCube4x4::find_white_edge_bar_for_colour(char colour) {
    std::vector<std::vector<std::vector<int>>> edge_bar_pairs = {{{1,2}, {33,34}}, {{4,8}, {65,66}}, {{7,11}, {81,82}}, {{13,14}, {49,50}}, {{52,56}, {71,75}}, {{55,59}, {84,88}}, {{61,62}, {17,18}}, {{36,40}, {87,91}}, {{39,43}, {68,72}}, {{20,24}, {77,78}}, {{23,27}, {93,94}}, {{29,30}, {45,46}}};

    for (const auto& edge_bar : edge_bar_pairs) {
        int a = edge_bar[0][0];
        int b = edge_bar[0][1];
        int c = edge_bar[1][0];
        int d = edge_bar[1][1];

        if (facelets[a] == 'W' && facelets[b] == 'W' && 
            facelets[c] == colour && facelets[d] == colour) {
            return {a, b};
        }

        if (facelets[c] == 'W' && facelets[d] == 'W' && 
            facelets[a] == colour && facelets[b] == colour) {
            return {c, d}; 
        }
    }

    return {};
}

std::vector<int> RubiksCube4x4::find_edge_by_colour(char colour1, char colour2) {
    std::vector<std::vector<std::vector<int>>> edge_pieces = {{{1,2}, {33,34}}, {{4,8}, {65,66}}, {{7,11}, {81,82}}, {{13,14}, {49,50}}, {{55,59}, {84,88}}, {{52,56}, {71,75}}, {{36,40}, {87,91}}, {{39,43}, {68,72}}};
    for (const auto& edge_pair : edge_pieces) {
        int a = edge_pair[0][0];
        int b = edge_pair[0][1];
        int c = edge_pair[1][0];
        int d = edge_pair[1][1];

        if (facelets[a] == colour1 && facelets[b] == colour1 &&
            facelets[c] == colour2 && facelets[d] == colour2) {
            return {a, b};
        }
        if (facelets[c] == colour1 && facelets[d] == colour1 &&
            facelets[a] == colour2 && facelets[b] == colour2) {
            return {c, d};
        }
    }

    return {};
}

std::vector<std::vector<std::vector<std::vector<int>>>> RubiksCube4x4::yellow_edge_bars_on_top() {
    std::vector<std::vector<std::vector<int>>> edge_pieces = {{{1,2}, {33,34}}, {{4,8}, {65,66}}, {{7,11}, {81,82}}, {{13,14}, {49,50}}};
    std::vector<std::vector<std::vector<int>>> top_edges = {};
    std::vector<std::vector<std::vector<int>>> non_top_edges = {};
    for (const auto& edge_pair : edge_pieces) {
        int a = edge_pair[0][0];
        int b = edge_pair[0][1];
        int c = edge_pair[1][0];
        int d = edge_pair[1][1];
        
        if (facelets[a] == 'Y' && facelets[b] == 'Y' && facelets[c] == facelets[d]) {
            top_edges.push_back(edge_pair);
        }
        if (facelets[c] == 'Y' && facelets[d] == 'Y' && facelets[a] == facelets[b]) {
            non_top_edges.push_back(edge_pair);
        }
    }

    return {top_edges, non_top_edges};
}

int RubiksCube4x4::find_white_corner_face_from_colours(char colour1, char colour2) {
    std::vector<std::vector<int>> corner_indexes = {{15,51,80}, {3,83,32}, {0,64,35}, {12,48,67}, {63,92,19}, {60,79,16}, {95,44,31}, {76,47,28}};
    
    for (const auto& corner : corner_indexes) {
        int a = corner[0], b = corner[1], c = corner[2];

        if ((facelets[a] == colour1 && facelets[b] == colour2 && facelets[c] == 'W') ||
            (facelets[a] == colour2 && facelets[b] == colour1 && facelets[c] == 'W')) {
            return c;
        }
        if ((facelets[b] == colour1 && facelets[c] == colour2 && facelets[a] == 'W') ||
            (facelets[b] == colour2 && facelets[c] == colour1 && facelets[a] == 'W')) {
            return a;
        }
        if ((facelets[c] == colour1 && facelets[a] == colour2 && facelets[b] == 'W') ||
            (facelets[c] == colour2 && facelets[a] == colour1 && facelets[b] == 'W')) {
            return b;
        }
    }
    
    return -1;
}

bool RubiksCube4x4::check_corner_correct(char colour1, char colour2) {
    if (facelets[63] == colour1 && facelets[92] == colour2 && facelets[19] == 'W') {
        return true;
    } else {
        return false;
    }
}

void RubiksCube4x4::apply_random_moves(bool pause) {
    std::vector<std::string> UD_moves = {"U", "U2", "U'", "Uw", "Uw2", "Uw'", "D", "D2", "D'", "Dw", "Dw2", "Dw'"};
    std::vector<std::string> FB_moves = {"F", "F2", "F'", "Fw", "Fw2", "Fw'", "B", "B2", "B'", "Bw", "Bw2", "Bw'"};
    std::vector<std::string> RL_moves = {"R", "R2", "R'", "Rw", "Rw2", "Rw'", "L", "L2", "L'", "Lw", "Lw2", "Lw'"};
    std::vector<std::vector<std::string>> move_groups = {UD_moves, FB_moves, RL_moves};
    std::random_device rd;
    std::mt19937 rng(rd());

    std::string scramble;
    int prev_group = -1;

    for (int i = 0; i < 40; i++) {
        int group;
        do {
            group = rng() % 3; // Pick a random group (0, 1, or 2)
        } while (group == prev_group); // Ensure it's not the same as the last group

        std::vector<std::string> selected_moves = move_groups[group];
        std::uniform_int_distribution<int> dist(0, selected_moves.size() - 1);
        std::string move = selected_moves[dist(rng)];

        if (!scramble.empty()) scramble += " ";
        scramble += move;
        prev_group = group;
    }

    std::cout << "Scramble: " << scramble << std::endl << '\n';

    std::istringstream iss(scramble);
    std::string move;
    if (pause) {
        while (iss >> move) {
            std::cout << "Move: " << move << std::endl << '\n';
            apply_move(move);
            display_cube();
            std::cout << "Press Enter to continue...";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    } else {
        while (iss >> move) {
            apply_move(move);
        }
        display_cube();
    }
}


void RubiksCube4x4::apply_moves(const std::vector<std::string> moves) {
    std::string move;
    
    for (const std::string& move : moves) {
            apply_move(move);
        
    }

}

std::vector<std::string> RubiksCube4x4::apply_opposite_moves(const std::vector<std::string> moves) {
    std::vector<std::string> opposite_moves;

    for (const std::string& move : moves) {
        opposite_moves.push_back(apply_opposite_move(move));
    }
    return opposite_moves;
}

std::string RubiksCube4x4::apply_opposite_move(const std::string move) {
    bool double_move = (move.back() == '2');
    bool clockwise = (move.back() != '\''); 
    std::string opposite_move;
    if (double_move) {
        opposite_move = move;
    } else if (clockwise) {
        opposite_move = move + "'";
    } else {
        opposite_move = move.substr(0, move.size() - 1);
    }
    apply_move(opposite_move);
    return opposite_move;
}

void RubiksCube4x4::apply_move(const std::string move) {
    bool clockwise = (move.back() != '\''); 

    std::string base_move = move;
    if (!clockwise) base_move = move.substr(0, move.size() - 1); // Remove ' if present
    int repeat = (base_move.back() == '2') ? 2 : 1;
    if (repeat == 2) base_move = base_move.substr(0, base_move.size() - 1); // Remove '2' if present

    std::cout << "Applying Move: " << move << "\n";
    for (int i = 0; i < repeat; i++) {
        if (base_move == "R") move_R(clockwise);
        else if (base_move == "r") move_r(clockwise);
        else if (base_move == "Rw") move_Rw(clockwise);
        else if (base_move == "L") move_L(clockwise);
        else if (base_move == "l") move_l(clockwise);
        else if (base_move == "Lw") move_Lw(clockwise);
        else if (base_move == "U") move_U(clockwise);
        else if (base_move == "u") move_u(clockwise);
        else if (base_move == "Uw") move_Uw(clockwise);
        else if (base_move == "D") move_D(clockwise);
        else if (base_move == "d") move_d(clockwise);
        else if (base_move == "Dw") move_Dw(clockwise);
        else if (base_move == "F") move_F(clockwise);
        else if (base_move == "f") move_f(clockwise);
        else if (base_move == "Fw") move_Fw(clockwise);
        else if (base_move == "B") move_B(clockwise);
        else if (base_move == "b") move_b(clockwise);
        else if (base_move == "Bw") move_Bw(clockwise);
        else if (base_move == "x") rotate_x(clockwise);
        else if (base_move == "y") rotate_y(clockwise);
        else if (base_move == "z") rotate_z(clockwise);
        else if (base_move == "");
        else std::cout << "Invalid move: " << move << "\n";
    }
}

void RubiksCube4x4::apply_index_swaps(const std::unordered_map<int, int> index_move_map, bool clockwise) {
    std::vector<char> facelets_copy = facelets;
    for (const auto& [a, b] : index_move_map) {
        if (clockwise) {
            facelets[a] = facelets_copy[b];
        } else {
            facelets[b] = facelets_copy[a];
        }
    }
}

bool RubiksCube4x4::verify_valid_cube() {
    std::cout << "Checking cube is valid: " << "\n";
    std::vector<char> colours = {'W', 'Y', 'O', 'R', 'G', 'B'};
    std::vector<int> pieces;
    for (const char& color : colours) {
        pieces = find_centre_pieces(color);
        if (pieces.size() != 4) {
            std::cout << "Not Valid (found " << pieces.size() << " " << color << " centre pieces instead of 4)\n";
            return false;
        }
        pieces = find_edge_pieces(color);
        if (pieces.size() != 8) {
            std::cout << "Not Valid (found " << pieces.size() << " " << color << " edge pieces instead of 8)\n";
            return false;
        }
        pieces = find_corner_pieces(color);
        if (pieces.size() != 4) {
            std::cout << "Not Valid (found " << pieces.size() << " " << color << " corner pieces instead of 4)\n";
            return false;
        }
    }
    std::cout << "Valid (all checks passed)\n";
    return true;
}


void RubiksCube4x4::rotate_x(bool clockwise) {
    move_Rw(clockwise);
    move_Lw(!clockwise);
}

void RubiksCube4x4::rotate_y(bool clockwise) {
    move_Uw(clockwise);
    move_Dw(!clockwise);
}

void RubiksCube4x4::rotate_z(bool clockwise) {
    move_Fw(clockwise);
    move_Bw(!clockwise);
}

void RubiksCube4x4::move_R(bool clockwise) {
    std::unordered_map<int, int> index_move_map = {
        {3, 51}, {7, 55}, {11, 59}, {15, 63}, {51, 19}, {55, 23}, {59, 27}, {63, 31}, {19, 44}, {23,40}, {27, 36}, {31, 32}, {44, 3}, {40, 7}, {36, 11}, {32,15},
        {80, 92}, {81, 88}, {82, 84}, {83, 80}, {84, 93}, {85, 89}, {86, 85}, {87, 81}, {88, 94}, {89, 90}, {90, 86}, {91, 82}, {92, 95}, {93, 91}, {94, 87}, {95, 83}
    };       
    apply_index_swaps(index_move_map, clockwise);
}

void RubiksCube4x4::move_r(bool clockwise) {
    std::unordered_map<int, int> index_move_map = {
        {2, 50}, {6, 54}, {10, 58}, {14, 62}, {50, 18}, {54, 22}, {58, 26}, {62, 30}, {18, 45}, {22,41}, {26, 37}, {30, 33}, {45, 2}, {41, 6}, {37, 10}, {33,14},
    };       
    apply_index_swaps(index_move_map, clockwise);
}

void RubiksCube4x4::move_Rw(bool clockwise){
    move_r(clockwise);
    move_R(clockwise);
}

void RubiksCube4x4::move_L(bool clockwise) {
    std::unordered_map<int, int> index_move_map = {
        {48, 0}, {52, 4}, {56, 8}, {60, 12}, {16, 48}, {20, 52}, {24, 56}, {28, 60}, {47, 16}, {43, 20}, {39, 24}, {35, 28}, {0, 47}, {4, 43}, {8, 39}, {12, 35},
        {64, 76}, {65, 72}, {66, 68}, {67, 64}, {68, 77}, {69, 73}, {70, 69}, {71, 65}, {72, 78}, {73, 74}, {74, 70}, {75, 66}, {76, 79}, {77, 75}, {78, 71}, {79, 67}
    };       
    apply_index_swaps(index_move_map, clockwise);
}

void RubiksCube4x4::move_l(bool clockwise) {
    std::unordered_map<int, int> index_move_map = {
        {49, 1}, {53, 5}, {57, 9}, {61, 13}, {17, 49}, {21, 53}, {25, 57}, {29, 61}, {46, 17}, {42, 21}, {38, 25}, {34, 29}, {1, 46}, {5, 42}, {9, 38}, {13, 34},
    };       
    apply_index_swaps(index_move_map, clockwise);
}

void RubiksCube4x4::move_Lw(bool clockwise){
    move_l(clockwise);
    move_L(clockwise);
}

void RubiksCube4x4::move_U(bool clockwise) {
    std::unordered_map<int, int> index_move_map = {
        {48, 80}, {49, 81}, {50, 82}, {51, 83}, {80, 32}, {81, 33}, {82, 34}, {83, 35}, {32, 64}, {33, 65}, {34, 66}, {35, 67}, {64, 48}, {65, 49}, {66, 50}, {67, 51},
        {0, 12}, {1, 8}, {2, 4}, {3, 0}, {4, 13}, {5, 9}, {6, 5}, {7, 1}, {8, 14}, {9, 10}, {10, 6}, {11, 2}, {12, 15}, {13, 11}, {14, 7}, {15, 3}
    };       
    apply_index_swaps(index_move_map, clockwise);
}

void RubiksCube4x4::move_u(bool clockwise) {
    std::unordered_map<int, int> index_move_map = {
        {52, 84}, {53, 85}, {54, 86}, {55, 87}, {84, 36}, {85, 37}, {86, 38}, {87, 39}, {36, 68}, {37, 69}, {38, 70}, {39, 71}, {68, 52}, {69, 53}, {70, 54}, {71, 55},
    };       
    apply_index_swaps(index_move_map, clockwise);
}

void RubiksCube4x4::move_Uw(bool clockwise){
    move_u(clockwise);
    move_U(clockwise);
}

void RubiksCube4x4::move_D(bool clockwise) {
    std::unordered_map<int, int> index_move_map = {
        {92, 60}, {93, 61}, {94, 62}, {95, 63}, {44, 92}, {45, 93}, {46, 94}, {47, 95}, {76, 44}, {77, 45}, {78, 46}, {79, 47}, {60, 76}, {61, 77}, {62, 78}, {63, 79},
        {16, 28}, {17, 24}, {18, 20}, {19, 16}, {20, 29}, {21, 25}, {22, 21}, {23, 17}, {24, 30}, {25, 26}, {26, 22}, {27, 18}, {28, 31}, {29, 27}, {30, 23}, {31, 19}
    };        
    apply_index_swaps(index_move_map, clockwise);
}

void RubiksCube4x4::move_d(bool clockwise) {
    std::unordered_map<int, int> index_move_map = {
        {88, 56}, {89, 57}, {90, 58}, {91, 59}, {40, 88}, {41, 89}, {42, 90}, {43, 91}, {72, 40}, {73, 41}, {74, 42}, {75, 43}, {56, 72}, {57, 73}, {58, 74}, {59, 75},
    };       
    apply_index_swaps(index_move_map, clockwise);
}

void RubiksCube4x4::move_Dw(bool clockwise){
    move_d(clockwise);
    move_D(clockwise);
}

void RubiksCube4x4::move_F(bool clockwise) {
    std::unordered_map<int, int> index_move_map = {
        {80, 12}, {84, 13}, {88, 14}, {92, 15}, {19, 80}, {18, 84}, {17, 88}, {16, 92}, {79, 19}, {75, 18}, {71, 17}, {67, 16}, {12, 79}, {13, 75}, {14, 71}, {15, 67},
        {48, 60}, {49, 56}, {50, 52}, {51, 48}, {52, 61}, {53, 57}, {54, 53}, {55, 49}, {56, 62}, {57, 58}, {58, 54}, {59, 50}, {60, 63}, {61, 59}, {62, 55}, {63, 51}
    };        
    apply_index_swaps(index_move_map, clockwise);
}

void RubiksCube4x4::move_f(bool clockwise) {
    std::unordered_map<int, int> index_move_map = {
        {81, 8}, {85, 9}, {89, 10}, {93, 11}, {23, 81}, {22, 85}, {21, 89}, {20, 93}, {66, 20}, {70, 21}, {74, 22}, {78, 23}, {8, 78}, {9, 74}, {10, 70}, {11, 66},
    };       
    apply_index_swaps(index_move_map, clockwise);
}

void RubiksCube4x4::move_Fw(bool clockwise){
    move_f(clockwise);
    move_F(clockwise);
}

void RubiksCube4x4::move_B(bool clockwise) {
    std::unordered_map<int, int> index_move_map = {
        {64, 3}, {68, 2}, {72, 1}, {76, 0}, {31, 76}, {30, 72}, {29, 68}, {28, 64}, {83, 31}, {87, 30}, {91, 29}, {95, 28}, {0, 83}, {1, 87}, {2, 91}, {3, 95},
        {32, 44}, {33, 40}, {34, 36}, {35, 32}, {36, 45}, {37, 41}, {38, 37}, {39, 33}, {40, 46}, {41, 42}, {42, 38}, {43, 34}, {44, 47}, {45, 43}, {46, 39}, {47, 35},
    };        
    apply_index_swaps(index_move_map, clockwise);
}

void RubiksCube4x4::move_b(bool clockwise) {
    std::unordered_map<int, int> index_move_map = {
        {65, 7}, {69, 6}, {73, 5}, {77, 4}, {27, 77}, {26, 73}, {25, 69}, {24, 65}, {82, 27}, {86, 26}, {90, 25}, {94, 24}, {4, 82}, {5, 86}, {6, 90}, {7, 94},
    };       
    apply_index_swaps(index_move_map, clockwise);
}

void RubiksCube4x4::move_Bw(bool clockwise){
    move_b(clockwise);
    move_B(clockwise);
}

