#include <vector>
#include <limits>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <functional>
#include <chrono>
#include "cube.h"
#include "BooPHF.h"
using namespace boomphf;

// IDA* recursive search function using table
static bool ida_search_multi(std::vector<Phase> &phases, Cube4x4 &cube, int depth, double &threshold, std::vector<Move> &path, double &next_threshold) {
    double h = 0.0;
    for (Phase &phase : phases) {
        std::string state = phase.mask(cube);
        uint64_t idx = phase.hash_table.lookup(state);
        double phase_h = (idx >= phase.depths.size()) ? (phase.table_depth_limit + 1) : static_cast<double>(static_cast<unsigned char>(phase.depths[idx]));
        h = h + phase_h;
    }

    double f = depth + h;
    if (f > threshold) {
        if (f < next_threshold) {
            next_threshold = f;
        }
        return false;
    }
    if (h == 0) {
        return true;
    }
    if (depth >= phases[0].search_depth_limit) {
        return false;
    }

    Cube4x4 backup = cube;
    for (Move mv : phases[0].moves) {
        cube.move(mv);
        path.push_back(mv);

        if (ida_search_multi(phases, cube, depth + 1, threshold, path, next_threshold)) {
            return true;
        }
        path.pop_back();
        cube = backup;
    }

    return false;
}

// IDA* recursive search function using table
static bool ida_search(Phase &phase, Cube4x4 &cube, int depth, double &threshold, std::vector<Move> &path, double &next_threshold) {
    std::string state = phase.mask(cube);
    uint64_t idx = phase.hash_table.lookup(state);

    double h = (idx >= phase.depths.size()) ? (phase.table_depth_limit + 1) : static_cast<double>(static_cast<unsigned char>(phase.depths[idx]));

    double f = depth + h;
    if (f > threshold) {
        if (f < next_threshold) {
            next_threshold = f;
        }
        return false;
    }
    if (h == 0) {
        return true;
    }
    if (depth >= phase.search_depth_limit) {
        return false;
    }

    Cube4x4 backup = cube;
    for (Move mv : phase.moves) {
        cube.move(mv);
        path.push_back(mv);

        if (ida_search(phase, cube, depth + 1, threshold, path, next_threshold)) {
            return true;
        }
        path.pop_back();
        cube = backup;
    }

    return false;
}

std::vector<Move> solve_any_phase_ida(Phase &phase, Cube4x4 start_cube) {
    std::vector<Move> path;
    std::string state = phase.mask(start_cube);
    uint64_t idx = phase.hash_table.lookup(state);

    double threshold = (idx >= phase.depths.size()) ? (phase.table_depth_limit + 1) : static_cast<double>(static_cast<unsigned char>(phase.depths[idx]));

    while (true) {
        double next_threshold = std::numeric_limits<double>::infinity();
        path.clear();

        bool found = ida_search(phase, start_cube, 0, threshold, path, next_threshold);
        if (found) {
            return path;
        }
        if (next_threshold == std::numeric_limits<double>::infinity()) {
            return {};
        }
        threshold = next_threshold;
    }
}

static bool ida_search_collect(Phase &phase, Cube4x4 &cube, int depth, double &threshold, std::vector<Move> &path, double &next_threshold, int &solutions_found, int max_solutions, std::vector<std::vector<Move>> &all_solutions) {
    std::string state = phase.mask(cube);
    uint64_t idx = phase.hash_table.lookup(state);
    double h = (idx >= phase.depths.size()) ? (phase.table_depth_limit + 1) : static_cast<double>(static_cast<unsigned char>(phase.depths[idx]));
    double f = depth + h;
    if (f > threshold) {
        if (f < next_threshold) { next_threshold = f; }
        return false;
    }
    if (h == 0) {
        all_solutions.push_back(path);
        solutions_found++;
        if (solutions_found >= max_solutions) { return true; }
    }
    if (depth >= phase.search_depth_limit) { return false; }
    Cube4x4 backup = cube;
    for (Move mv : phase.moves) {
        cube.move(mv);
        path.push_back(mv);
        bool filled = ida_search_collect(phase, cube, depth + 1, threshold, path, next_threshold, solutions_found, max_solutions, all_solutions);
        path.pop_back();
        cube = backup;
        if (filled) { return true; }
    }
    return false;
}

std::vector<std::vector<Move>> collect_multiple_solutions(Phase &phase, Cube4x4 start_cube, int how_many) {
    std::vector<std::vector<Move>> solutions;
    solutions.reserve(how_many);
    std::string state = phase.mask(start_cube);
    uint64_t idx = phase.hash_table.lookup(state);
    double threshold = (idx >= phase.depths.size()) ? (phase.table_depth_limit + 1) : static_cast<double>(static_cast<unsigned char>(phase.depths[idx]));
    while ((int)solutions.size() < how_many) {
        double next_threshold = std::numeric_limits<double>::infinity();
        std::vector<Move> path;
        int solutions_found_before = (int)solutions.size();
        ida_search_collect(phase, start_cube, 0, threshold, path, next_threshold, solutions_found_before, how_many, solutions);
        if ((int)solutions.size() == solutions_found_before) {
            if (next_threshold == std::numeric_limits<double>::infinity()) { break; }
            threshold = next_threshold;
        } else {
            if ((int)solutions.size() >= how_many) { break; }
        }
    }
    return solutions;
}

std::vector<char> load_depths(const std::string &filename) {
    std::ifstream in(filename, std::ios::binary);
    uint64_t sz = 0;
    in.read(reinterpret_cast<char*>(&sz), sizeof(sz));
    std::vector<char> depths(sz);
    in.read(reinterpret_cast<char*>(depths.data()), sz);
    return depths;
}

boomphf::mphf<std::string, MyStringHash> load_hash_table(const std::string &filename) {
    boomphf::mphf<std::string, MyStringHash> mph;
    std::ifstream ifs(filename, std::ios::binary);
    mph.load(ifs);
    return mph;
}

int lookup_depth(const std::string &state, const std::string &mph_file, const std::string &depth_file) {
    auto mph = new boomphf::mphf<std::string, MyStringHash>();
    std::ifstream ifs(mph_file, std::ios::binary);
    mph->load(ifs);
    auto depths = load_depths(depth_file);
    uint64_t idx = mph->lookup(state);
    delete mph;
    if (idx >= depths.size()) {
        return -1;
    }
    return depths[idx];
}

static std::pair<double,double> compute_mean_std(const std::vector<double>& vals)
{
    if(vals.empty()) {
        return {0.0, 0.0};
    }
    double sum = 0.0;
    for(double v : vals) {
        sum += v;
    }
    double mean = sum / vals.size();

    double sqSum = 0.0;
    for(double v : vals) {
        double diff = v - mean;
        sqSum += diff * diff;
    }
    double stdev = std::sqrt(sqSum / vals.size());
    return {mean, stdev};
}

// Print and apply solution to the cube
void print_apply_solution(Cube4x4 &cube, const std::vector<Move> &solution, std::string name) {
    // std::cout << name << " solution found in " << solution.size() << " moves: ";
    for (Move move : solution) {
        std::cout << move_to_string(move) << std::endl;
        cube.move(move);
    }
    std::cout << std::endl;
    // cube.print();
    // std::cout << "Edge parities: " << cube.edge_piece_permutation_parity() << std::endl;
    // std::cout << name << " done.\n" << std::endl;
    // std::string filename = name;
    // filename.erase(std::remove(filename.begin(), filename.end(), ' '), filename.end());
    // filename += ".png";
    // cube.export_flat_image(filename);
}

int main(int argc, char* argv[]) {\
    Cube4x4 cube;

    if (argc == 1) {
        std::vector<Move> scramble = cube.apply_random_moves(35, {R,L,F,B,U,D,r,l,f,b,u,d});
        // std::cout << "Scramble of size " << scramble.size() << " moves: ";
        // for (Move move : scramble) { std::cout << move_to_string(move) << std::endl; }
    } else if (argc == 2 && std::string(argv[1]) == "-s") {
        // Get cube state input later
    } else if (argc == 3 && std::string(argv[1]) == "-m") {
        std::string moves_string = argv[2];
        std::vector<std::string> moves;
        size_t start = 0;
        size_t end;
        while ((end = moves_string.find(' ', start)) != std::string::npos) {
            moves.push_back(moves_string.substr(start, end - start));
            start = end + 1;
        }
        if (start < moves_string.length()) {
            moves.push_back(moves_string.substr(start));
        }
        Cube4x4 cube;
        for (auto move : moves) {
            cube.move(cube.string_to_move(move));
        }
        std::cout << cube.export_state() << std::endl;
        return 0;
    } else {
        std::cerr << "Usage: ./solver -[s/m]" << std::endl;
        return 1;
    }

    // Initalise phase objects
    Phase p1 = phase1;
    Phase p2a = phase2a;
    Phase p2b = phase2b;
    Phase p3 = phase3;
    Phase p4 = phase4;
    Phase p6 = phase6;
    Phase p7 = phase7;
    Phase p8 = phase8;
    std::vector<Phase*> phases = {&p1, &p2a, &p2b, &p3, &p4, &p6, &p7, &p8};

    // Load phase hash tables in
    for (Phase* phase : phases) {
        // std::cout << "Loading " << phase->name << " table..." << std::endl;;
        std::string mphFile = phase->table_filename + ".mph";
        std::string depthFile = phase->table_filename + ".depths";
        auto mph = load_hash_table(mphFile);
        auto dvec = load_depths(depthFile);
        phase->set_hash_table(mph);
        phase->set_depths(dvec);
    }

    if (argc == 2 && std::string(argv[1]) == "-s") {
        std::cout << "Enter cube state to continue ..." << std::endl;
        std::string cube_state;
        std::getline(std::cin, cube_state);
        cube.check_state_is_valid(cube_state);
        cube.import_state(cube_state);
        cube.valid_facelets_ids();
    }
    

    // cube.print();

    static const std::unordered_map<std::string, double> move_to_time = {
        {"R", 0.32}, {"R'", 0.32}, {"R2", 0.34},
        {"L", 0.32}, {"L'", 0.32}, {"L2", 0.34},
        {"U", 1.15}, {"U'", 1.15}, {"U2", 1.17},
        {"D", 1.15}, {"D'", 1.15}, {"D2", 1.17},
        {"F", 0.32}, {"F'", 0.32}, {"F2", 0.34},
        {"B", 0.32}, {"B'", 0.32}, {"B2", 0.34},
        {"r", 0.54}, {"r'", 0.54}, {"r2", 0.59},
        {"l", 0.54}, {"l'", 0.54}, {"l2", 0.59},
        {"u", 1.37}, {"u'", 1.37}, {"u2", 1.42},
        {"d", 1.37}, {"d'", 1.37}, {"d2", 1.42},
        {"f", 0.54}, {"f'", 0.54}, {"f2", 0.59},
        {"b", 0.54}, {"b'", 0.54}, {"b2", 0.59},
        {"Rw", 0.29}, {"Rw'", 0.29}, {"Rw2", 0.32},
        {"Lw", 0.29}, {"Lw'", 0.29}, {"Lw2", 0.32},
        {"Uw", 1.10}, {"Uw'", 1.10}, {"Uw2", 1.12},
        {"Dw", 1.10}, {"Dw'", 1.10}, {"Dw2", 1.12},
        {"Fw", 0.29}, {"Fw'", 0.29}, {"Fw2", 0.32},
        {"Bw", 0.29}, {"Bw'", 0.29}, {"Bw2", 0.32},
    };

    // Start solver
    int n = 100;
    int count = 0;
    std::vector<double> totalMoves(n, 0.0);
    std::vector<double> solveTimes(n, 0.0); 
    std::vector<double> robotTimes(n, 0.0); 

    std::vector<double> phase1Moves(n, 0.0); 
    std::vector<double> phase2aMoves(n, 0.0); 
    std::vector<double> phase2bMoves(n, 0.0); 
    std::vector<double> phase3Moves(n, 0.0); 
    std::vector<double> phase4Moves(n, 0.0); 
    std::vector<double> phase6Moves(n, 0.0); 
    std::vector<double> phase7Moves(n, 0.0); 
    std::vector<double> phase8Moves(n, 0.0); 

    while(count < n) {
        // std::cout << "\n--- Trial " << (i+1) << " of " << n << " ---\n";

        std::vector<Move> final_solution;
        auto start_all = std::chrono::high_resolution_clock::now();
        bool found_solution = false;

        std::vector<std::vector<Move>> p1_solutions = collect_multiple_solutions(p1, cube, 100);
        for (auto &p1_solution : p1_solutions) {
            Cube4x4 after_p1 = cube;
            for (auto mv : p1_solution) { after_p1.move(mv); }

            std::vector<std::vector<Move>> p2a_solutions = collect_multiple_solutions(p2a, after_p1, 10);
            for (auto &p2a_solution : p2a_solutions) {
                Cube4x4 after_p2a = after_p1;
                for (auto mv : p2a_solution) { after_p2a.move(mv); }

                std::vector<std::vector<Move>> p2b_solutions = collect_multiple_solutions(p2b, after_p2a, 10);
                for (auto &p2b_solution : p2b_solutions) {
                    Cube4x4 after_p2b = after_p2a;
                    for (auto mv : p2b_solution) { after_p2b.move(mv); }

                    if (after_p2b.edge_piece_permutation_parity() == 1) {
                        p1.set_solution(p1_solution);
                        p2a.set_solution(p2a_solution);
                        p2b.set_solution(p2b_solution);
                        found_solution = true;
                        break;
                    }
                }
                if (found_solution) break;
            }
            if (found_solution) break;
        }        
        print_apply_solution(cube, p1.solution, p1.name);
        print_apply_solution(cube, p2a.solution, p2a.name);
        print_apply_solution(cube, p2b.solution, p2b.name);
        
        std::vector<Move> p3_solution = solve_any_phase_ida(p3, cube);
        p3.set_solution(p3_solution);
        print_apply_solution(cube, p3.solution, p3.name);
        
        std::vector<Move> p4_solution = solve_any_phase_ida(p4, cube);
        p4.set_solution(p4_solution);
        print_apply_solution(cube, p4.solution, p4.name);

        // std::vector<Move> p6_solution = solve_any_phase_ida(p6, cube);
        // p6.set_solution(p6_solution);
        // print_apply_solution(cube, p6.solution, p6.name);
        
        // std::vector<Move> p7_solution = solve_any_phase_ida(p7, cube);
        // p7.set_solution(p7_solution);
        // print_apply_solution(cube, p7.solution, p7.name);

        // std::vector<Move> p8_solution = solve_any_phase_ida(p8, cube);
        // p8.set_solution(p8_solution);
        // print_apply_solution(cube, p8.solution, p8.name);
        

        std::vector<std::vector<Move>> p6_candidates = collect_multiple_solutions(p6, cube, 5);
        std::vector<Move> bad_moves = {U, U_PRIME, U2, D, D_PRIME, D2};
        int best_total_size = std::numeric_limits<int>::max();

        for (auto &candidate_p6 : p6_candidates) {
            Cube4x4 after_p6 = cube;
            for (auto mv : candidate_p6) { after_p6.move(mv); }
            std::vector<std::vector<Move>> p7_candidates = collect_multiple_solutions(p7, after_p6, 5);

            for (auto &candidate_p7 : p7_candidates) {
                Cube4x4 after_p7 = after_p6;
                for (auto mv : candidate_p7) { after_p7.move(mv); }
                std::vector<std::vector<Move>> p8_candidates = collect_multiple_solutions(p8, after_p7, 5);

                for (auto &candidate_p8 : p8_candidates) {
                    auto penalised_size = [&](const std::vector<Move>& moves) {
                        int size = 0;
                        for (auto mv : moves) {
                            size += std::find(bad_moves.begin(), bad_moves.end(), mv) != bad_moves.end() ? 4 : 1;
                        }
                        return size;
                    };
                    int total_size = penalised_size(candidate_p6) + penalised_size(candidate_p7) + penalised_size(candidate_p8);                

                    if (total_size < best_total_size) {
                        best_total_size = total_size;
                        p6.set_solution(candidate_p6);
                        p7.set_solution(candidate_p7);
                        p8.set_solution(candidate_p8);
                    }
                }
            }
        }

        print_apply_solution(cube, p6.solution, p6.name);
        print_apply_solution(cube, p7.solution, p7.name);
        print_apply_solution(cube, p8.solution, p8.name);

        if (cube.check_solved()) {
            //Output
            // std::cout << "***** RESULTS *****" << std::endl;
            for (Phase* phase : phases) {
                final_solution.insert(final_solution.end(), phase->solution.begin(), phase->solution.end());
                // std::cout << phase->name << ", " << phase->solution.size() << " moves, " << phase->time << "s." << std::endl;
            }
            auto end_all = std::chrono::high_resolution_clock::now();
            double total_time = std::chrono::duration<double>(end_all - start_all).count();
            solveTimes[count] = total_time;
            totalMoves[count] = final_solution.size();
            phase1Moves[count] = p1.solution.size();
            phase2aMoves[count] = p2a.solution.size();
            phase2bMoves[count] = p2b.solution.size();
            phase3Moves[count] = p3.solution.size();
            phase4Moves[count] = p4.solution.size();
            phase6Moves[count] = p6.solution.size();
            phase7Moves[count] = p7.solution.size();
            phase8Moves[count] = p8.solution.size();
            
            // std::cout << "Total solution, " << final_solution.size() << " moves, " << total_time << "s" << std::endl;
            // std::cout << "Edge/corner parity: " << cube.edge_corner_permutation_parity() << std::endl;  
            double robot_time = 0.0;
            for (Move move : final_solution) { 
                if (auto it = move_to_time.find(move_to_string(move)); it != move_to_time.end()) {
                    robot_time = robot_time + it->second + 0.1;
                }
            }
            robotTimes[count] = robot_time;
            count += 1;
        } else {
            std::cout << "Solution Failed" << std::endl;
        }

        cube.apply_random_moves(35, {R,L,F,B,U,D,r,l,f,b,u,d});
    }

    auto [timeMean, timeStd] = compute_mean_std(solveTimes);
    auto [movesMean, movesStd] = compute_mean_std(totalMoves);
    auto [robotTimeMean, robotTimeStd] = compute_mean_std(robotTimes);
    auto [phase1Mean, phase1Std] = compute_mean_std(phase1Moves);
    auto [phase2aMean, phase2aStd] = compute_mean_std(phase2aMoves);
    auto [phase2bMean, phase2bStd] = compute_mean_std(phase2bMoves);
    auto [phase3Mean, phase3Std] = compute_mean_std(phase3Moves);
    auto [phase4Mean, phase4Std] = compute_mean_std(phase4Moves);
    auto [phase6Mean, phase6Std] = compute_mean_std(phase6Moves);
    auto [phase7Mean, phase7Std] = compute_mean_std(phase7Moves);
    auto [phase8Mean, phase8Std] = compute_mean_std(phase8Moves);
    // std::cout << "\n===== AVERAGE RESULTS OVER " << solveTimes.size() << " TRIALS =====\n";
    // std::cout << "Total Moves        : Mean = " << movesMean << ", Std Dev = " << movesStd << std::endl;
    // std::cout << "Solve Time (s)     : Mean = " << timeMean << ", Std Dev = " << timeStd << std::endl;
    // std::cout << "Robot Time (s)     : Mean = " << robotTimeMean << ", Std Dev = " << robotTimeStd << std::endl;
    // std::cout << "Phase 1 Moves      : Mean = " << phase1Mean << ", Std Dev = " << phase1Std << std::endl;
    // std::cout << "Phase 2a Moves     : Mean = " << phase2aMean << ", Std Dev = " << phase2aStd << std::endl;
    // std::cout << "Phase 2b Moves     : Mean = " << phase2bMean << ", Std Dev = " << phase2bStd << std::endl;
    // std::cout << "Phase 3 Moves      : Mean = " << phase3Mean << ", Std Dev = " << phase3Std << std::endl;
    // std::cout << "Phase 4 Moves      : Mean = " << phase4Mean << ", Std Dev = " << phase4Std << std::endl;
    // std::cout << "Phase 6 Moves      : Mean = " << phase6Mean << ", Std Dev = " << phase6Std << std::endl;
    // std::cout << "Phase 7 Moves      : Mean = " << phase7Mean << ", Std Dev = " << phase7Std << std::endl;
    // std::cout << "Phase 8 Moves      : Mean = " << phase8Mean << ", Std Dev = " << phase8Std << std::endl;

    return 0;
}