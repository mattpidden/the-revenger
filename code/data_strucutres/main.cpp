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
    static constexpr int BITS_PER_FACELET = 3; 
    static constexpr int TOTAL_BITS = FACELETS * BITS_PER_FACELET;

    std::bitset<TOTAL_BITS> facelets;

    // Constructor: make a solved cube
    Cube4x4() {
        reset();
    }

    // Reset to goal state:
    void reset() {
        for (int i = 0; i < FACELETS; i++) {
            setFacelet(i, static_cast<Colour>(i / 16));
        }
    }

    void move_R(bool clockwise) {
        Colour temp3 = getFacelet(3);
        Colour temp7 = getFacelet(3);
        Colour temp11 = getFacelet(11);
        Colour temp15 = getFacelet(15);
        setFacelet(3, getFacelet(51));
        setFacelet(7, getFacelet(55));
        setFacelet(11, getFacelet(59));
        setFacelet(15, getFacelet(63));
        setFacelet(51, getFacelet(19));
        setFacelet(55, getFacelet(23));
        setFacelet(59, getFacelet(27));
        setFacelet(63, getFacelet(31));
        setFacelet(19, getFacelet(44));
        setFacelet(23, getFacelet(40));
        setFacelet(27, getFacelet(36));
        setFacelet(31, getFacelet(32));
        setFacelet(44, temp3);
        setFacelet(40, temp7);
        setFacelet(36, temp11);
        setFacelet(32, temp15);
        Colour temp80 = getFacelet(80);
        Colour temp81 = getFacelet(81);
        Colour temp82 = getFacelet(82);
        Colour temp83 = getFacelet(83);
        Colour temp85 = getFacelet(85);
        Colour temp87 = getFacelet(87);
        Colour temp91 = getFacelet(91);
        setFacelet(80, getFacelet(92));
        setFacelet(81, getFacelet(88));
        setFacelet(82, getFacelet(84));
        setFacelet(83, temp80);
        setFacelet(84, getFacelet(93));
        setFacelet(85, getFacelet(89));
        setFacelet(86, temp85);
        setFacelet(87, temp81);
        setFacelet(88, getFacelet(94));
        setFacelet(89, getFacelet(90));
        setFacelet(90, getFacelet(86));
        setFacelet(91, temp82);
        setFacelet(92, getFacelet(95));
        setFacelet(93, temp91);
        setFacelet(94, temp87);
        setFacelet(95, temp83);
    }

    void move_r(bool clockwise) {
        Colour temp2 = getFacelet(2);
        Colour temp6 = getFacelet(6);
        Colour temp10 = getFacelet(10);
        Colour temp14 = getFacelet(14);
        setFacelet(2, getFacelet(50));
        setFacelet(6, getFacelet(54));
        setFacelet(10, getFacelet(58));
        setFacelet(14, getFacelet(62));
        setFacelet(50, getFacelet(18));
        setFacelet(54, getFacelet(22));
        setFacelet(58, getFacelet(26));
        setFacelet(62, getFacelet(30));
        setFacelet(18, getFacelet(45));
        setFacelet(22, getFacelet(41));
        setFacelet(26, getFacelet(37));
        setFacelet(30, getFacelet(33));
        setFacelet(45, temp2);
        setFacelet(41, temp6);
        setFacelet(37, temp10);
        setFacelet(33, temp14);
    }

    Colour getFacelet(int index) const {
        int bitIndex = index * BITS_PER_FACELET;
        int value = 0;
        
        for (int i = 0; i < BITS_PER_FACELET; i++) {
            if (facelets.test(bitIndex + i)) { 
                value |= (1 << i);  
            }
        }
        return static_cast<Colour>(value); 
    }

    void setFacelet(int index, Colour color) {
        int bitIndex = index * BITS_PER_FACELET;
        for (int i = 0; i < BITS_PER_FACELET; i++) {
            facelets.set(bitIndex + i, (color >> i) & 1);
        }
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

        std::cout << "Trial " << (trial + 1) << ":\n";
        std::cout << "Outer Slice Turn: " << outer_time_ms << " ms, " << outer_moves_per_sec << " moves/sec\n";
        std::cout << "Inner Slice Turn: " << inner_time_ms << " ms, " << inner_moves_per_sec << " moves/sec\n\n";

        csvFile << outer_time_ms << "," << outer_moves_per_sec << "," << inner_time_ms << "," << inner_moves_per_sec << "\n";
    }

    csvFile.close();
    std::cout << "Benchmark results saved to benchmark_results.csv\n";

    return 0;
}