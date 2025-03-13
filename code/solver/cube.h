#ifndef CUBE_H
#define CUBE_H

#include <array>
#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>
#include <functional>
#include <map>
#include "BooPHF.h"
#include <fstream>
#include <cstdint>

using namespace boomphf;

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

struct MyStringHash {
    // BooPHF calls operator()(key, seed)
    inline uint64_t operator()(const std::string &str, uint64_t seed = 0) const {
        static std::hash<std::string> stdhasher;
        uint64_t val = stdhasher(str);
        // Combine with seed
        val ^= seed + 0x9e3779b97f4a7c15ULL + (val << 6) + (val >> 2);
        return val;
    }
};

std::string move_to_string(Move move);

class BloomFilter {
public:
    BloomFilter(size_t size_in_bits, int num_hashes)
        : bitvec((size_in_bits + 7) / 8, 0), // round up to full bytes
            size(size_in_bits),
            num_hashes(num_hashes)
    {}

    BloomFilter() : bitvec(), size(0), num_hashes(0) {}


    // Add a string to the bloom filter
    void add(const std::string &s) {
        for (int i = 0; i < num_hashes; i++) {
            uint64_t h = hash_with_seed(s, i);
            uint64_t bit_index = h % size;
            // set that bit
            bitvec[bit_index / 8] |= (1 << (bit_index % 8));
        }
    }

    // Check if possibly contains the string
    bool contains(const std::string &s) const {
        for (int i = 0; i < num_hashes; i++) {
            uint64_t h = hash_with_seed(s, i);
            uint64_t bit_index = h % size;
            if ( (bitvec[bit_index / 8] & (1 << (bit_index % 8))) == 0) {
                // if any bit is not set, definitely not in filter
                return false;
            }
        }
        return true;
    }

    // Serialize to file
    void save(const std::string &filename) const {
        std::ofstream out(filename, std::ios::binary);
        // 1) write size, num_hashes
        out.write(reinterpret_cast<const char*>(&size), sizeof(size));
        out.write(reinterpret_cast<const char*>(&num_hashes), sizeof(num_hashes));
        // 2) write bitvector
        uint64_t vecsize = bitvec.size();
        out.write(reinterpret_cast<const char*>(&vecsize), sizeof(vecsize));
        out.write(reinterpret_cast<const char*>(bitvec.data()), vecsize);
    }

    // Load from file
    void load(const std::string &filename) {
        std::ifstream in(filename, std::ios::binary);
        in.read(reinterpret_cast<char*>(&size), sizeof(size));
        in.read(reinterpret_cast<char*>(&num_hashes), sizeof(num_hashes));
        uint64_t vecsize = 0;
        in.read(reinterpret_cast<char*>(&vecsize), sizeof(vecsize));
        bitvec.resize(vecsize);
        in.read(reinterpret_cast<char*>(bitvec.data()), vecsize);
    }

    // A trivial hash that uses std::hash<std::string>, combined with the seed
    // so each 'i' in [0..num_hashes-1] yields a different 64-bit value.
    uint64_t hash_with_seed(const std::string &s, int i) const {
        static std::hash<std::string> hasher;
        uint64_t h = hasher(s);
        // Mix in 'i' to differentiate each hash function
        h ^= (static_cast<uint64_t>(i) * 0x9e3779b97f4a7c15ULL);
        return h;
    }

    std::vector<unsigned char> bitvec; // the bits
    uint64_t size;     // number of bits
    int num_hashes;
};

class Phase {
public:
    std::string name;
    std::vector<Move> moves;
    std::function<std::string(Cube4x4&)> mask;
    std::string table_filename;
    int table_depth_limit;
    int search_depth_limit;
    boomphf::mphf<std::string, MyStringHash> hash_table;
    std::vector<int> depths;
    BloomFilter bloom;

    Phase(const std::string& name, const std::vector<Move>& moves, std::function<std::string(Cube4x4&)> mask, const std::string& table_filename, int table_depth_limit, int search_depth_limit) : name(name), moves(moves), mask(mask), table_filename(table_filename), table_depth_limit(table_depth_limit), search_depth_limit(search_depth_limit) {}

    void set_hash_table(boomphf::mphf<std::string, MyStringHash>& new_table) {
        hash_table = new_table;
    }
    void set_depths(std::vector<int>& new_depths) {
        depths = new_depths;
    }
    void set_bloom(BloomFilter new_bloom) {
        bloom = new_bloom;
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
