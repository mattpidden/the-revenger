#include <map>
#include <algorithm>
#include <utility>
#include <chrono>
#include <sys/resource.h>

#include "six_4x4_cube.h"



int main() {
    RubiksCube4x4 cube;

    std::vector<std::string> random_moves = cube.generate_random_moves(10000);

    auto start = std::chrono::high_resolution_clock::now();
    cube.apply_moves(random_moves);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Execution time: " << duration.count() << " seconds" << std::endl;
    
    

    return 0;
}

// valgrind --tool=massif ./cube 
// peak_memory_kb=$(( $(grep "mem_heap_B=" massif.out.* | sort -nr -t= -k2 | head -n 1 | cut -d= -f2) / 1024 ))
// echo "Peak Memory: ${peak_memory_kb} KB"


// 96 element vector of chars:
    // Execution time: 6.27695 seconds
    // Peak Memory: 7374 KB

// 
