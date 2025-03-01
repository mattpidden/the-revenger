#include <array>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <algorithm>
#include <map>
#include <chrono>
#include <bitset>
#include <malloc.h>

enum Colour : uint8_t{
    WHITE = 0, ORANGE, GREEN, RED, BLUE, YELLOW, 
};

enum Face {
    U_FACE = 0,
    L_FACE,
    F_FACE,
    R_FACE,
    B_FACE,
    D_FACE
};

enum Move {
    R, R_PRIME, R2,
    L, L_PRIME, L2,
    U, U_PRIME, U2,
    D, D_PRIME, D2,
    F, F_PRIME, F2,
    B, B_PRIME, B2,
    r, r_PRIME, r2,
    l, l_PRIME, l2,
    u, u_PRIME, u2,
    d, d_PRIME, d2,
    f, f_PRIME, f2,
    b, b_PRIME, b2,
};


class Cube4x4 {
public:
    static constexpr int FACELETS = 96;     
    static constexpr int CUBLETS = 56;

    std::array<uint8_t, CUBLETS> cublets;
    std::array<uint8_t, CUBLETS> orientations;


    // Constructor: make a solved cube
    Cube4x4() {
        reset();
    }

    // Reset to goal state:
    void reset() {
        for (int i = 0; i < CUBLETS; i++) {
            cublets[i] = i;
        }
        orientations.fill(0);
    }

    void move_R(bool clockwise) {
        auto tempC = cublets[15];
        auto tempCO = orientations[15];
        cublets[15] = cublets[55];
        orientations[15] = orientations[55] + 100;
        cublets[55] = cublets[43];
        orientations[55] = orientations[43] + 100;
        cublets[43] = cublets[3];
        orientations[43] = orientations[3] + 100;
        cublets[3] = tempC;
        orientations[3] = tempCO + 100;
        auto tempE1 = cublets[7];
        auto tempE1O = orientations[7];
        cublets[7] = cublets[27];
        orientations[7] = orientations[27] + 100;
        cublets[27] = cublets[51];
        orientations[27] = orientations[51] + 100;
        cublets[51] = cublets[31];
        orientations[51] = orientations[31] + 100;
        cublets[31] = tempE1;
        orientations[31] = tempE1O + 100;
        auto tempE2 = cublets[11];
        auto tempE2O = orientations[11];
        cublets[11] = cublets[39];
        orientations[11] = orientations[39] + 100;
        cublets[39] = cublets[47];
        orientations[39] = orientations[47] + 100;
        cublets[47] = cublets[19];
        orientations[47] = orientations[19] + 100;
        cublets[19] = tempE2;
        orientations[19] = tempE2O + 100;
        auto temp = cublets[21];
        auto tempO = orientations[21];
        cublets[21] = cublets[23];
        orientations[21] = orientations[23] + 100;
        cublets[23] = cublets[35];
        orientations[23] = orientations[35] + 100;
        cublets[35] = cublets[33];
        orientations[35] = orientations[33] + 100;
        cublets[33] = temp;
        orientations[33] = tempO + 100;
    }

    void move_r(bool clockwise) {
        auto tempC = cublets[14];
        auto tempCO = orientations[14];
        cublets[14] = cublets[55];
        orientations[14] = orientations[55] + 100;
        cublets[55] = cublets[43];
        orientations[55] = orientations[43] + 100;
        cublets[43] = cublets[3];
        orientations[43] = orientations[3] + 100;
        cublets[3] = tempC;
        orientations[3] = tempCO + 100;
        auto tempE1 = cublets[7];
        auto tempE1O = orientations[7];
        cublets[7] = cublets[27];
        orientations[7] = orientations[27] + 100;
        cublets[27] = cublets[51];
        orientations[27] = orientations[51] + 100;
        cublets[51] = cublets[31];
        orientations[51] = orientations[31] + 100;
        cublets[31] = tempE1;
        orientations[31] = tempE1O + 100;
        auto tempE2 = cublets[11];
        auto tempE2O = orientations[11];
        cublets[11] = cublets[39];
        orientations[11] = orientations[39] + 100;
        cublets[39] = cublets[47];
        orientations[39] = orientations[47] + 100;
        cublets[47] = cublets[19];
        orientations[47] = orientations[19] + 100;
        cublets[19] = tempE2;
        orientations[19] = tempE2O + 100;
    }



private:


};

int main() {
    Cube4x4 cube;
    int n = 1000000;
    const int num_trials = 10;

    std::ofstream csvFile("benchmark_results.csv");
    csvFile << "Outer Slice Turn, , ,Inner Slice Turn, \n";
    csvFile << "Time (ms),Moves/sec (m),Time (ms),Moves/sec (m)\n";

    for (int trial = 0; trial < num_trials; trial++) {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < n; i++) {
            cube.move_R(false);
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration = end - start;
        double outer_time_ms = duration.count();
        double outer_moves_per_sec = (n / outer_time_ms) * 1000.0 / 1e6;

        start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < n; i++) {
            cube.move_r(false);
        }
        end = std::chrono::high_resolution_clock::now();
        duration = end - start;
        double inner_time_ms = duration.count();
        double inner_moves_per_sec = (n / inner_time_ms) * 1000.0 / 1e6;

        // Benchmark memory
        struct mallinfo before = mallinfo();
        std::vector<Cube4x4*> cubes;
        cubes.reserve(n);
        for (int i = 0; i < n; i++) {
            cubes.push_back(new Cube4x4());
        }
        struct mallinfo after = mallinfo(); 
        size_t actual_memory_used_n = after.uordblks - before.uordblks;
        for (auto cube : cubes) {
            delete cube;
        }

        std::cout << "Trial " << (trial + 1) << ":\n";
        std::cout << "Outer Slice Turn: " << outer_time_ms << " ms, " << outer_moves_per_sec << " moves/sec\n";
        std::cout << "Inner Slice Turn: " << inner_time_ms << " ms, " << inner_moves_per_sec << " moves/sec\n";
        std::cout << "Heap memory used for " << n << " cubes: " << actual_memory_used_n / (1024.0 * 1024.0) << " MB\n\n";

        csvFile << outer_time_ms << "," << outer_moves_per_sec << "," << inner_time_ms << "," << inner_moves_per_sec << "\n";
    }

    csvFile.close();
    std::cout << "Benchmark results saved to benchmark_results.csv\n";

    return 0;
}