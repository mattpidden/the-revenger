#include <queue>
#include <stack>
#include <climits>
#include <unordered_set>

#include "tree.h"

std::vector<Move> solve_cube_bfs(Cube4x4 start_cube, int max_depth) {
    std::queue<TreeNode*> queue;
    TreeNode* root = new TreeNode(start_cube); 
    queue.push(root);

    std::vector<Move> all_moves = {
        R, L, U, D, F, B, R_PRIME, L_PRIME, U_PRIME, D_PRIME, F_PRIME, B_PRIME,
        r, l, u, d, f, b, r_PRIME, l_PRIME, u_PRIME, d_PRIME, f_PRIME, b_PRIME
    };

    while (!queue.empty()) {
        TreeNode* current = queue.front();
        queue.pop();

        if (current->get_cube().check_goal_state()) {
            return current->get_solution_path();
        }

        int depth = 0;
        TreeNode* temp = current;
        while (temp->get_parent()) {
            temp = temp->get_parent();
            depth++;
        }
        if (depth >= max_depth) continue;

        for (Move move : all_moves) {
            Cube4x4 new_cube = current->get_cube();
            new_cube.move(move);
            TreeNode* child = current->add_child(new_cube, move);
            queue.push(child);
        }
    }

    return {};
}

struct CubeHash {
    size_t operator()(const Cube4x4 &c) const {
        return std::hash<std::string>()(c.export_state()); 
    }
};

struct CubeEqual {
    bool operator()(const Cube4x4 &a, const Cube4x4 &b) const {
        return a.export_state() == b.export_state(); 
    }
};


std::vector<Move> depth_limited_dfs_no_duplicates(const Cube4x4 &start_cube, int depth_limit)
{
    std::stack<TreeNode*> stack;
    TreeNode* root = new TreeNode(start_cube);
    stack.push(root);

    std::unordered_set<Cube4x4, CubeHash, CubeEqual> visited;
    visited.insert(start_cube);

    std::vector<Move> all_moves = {
        R, L, U, D, F, B, R_PRIME, L_PRIME, U_PRIME, D_PRIME, F_PRIME, B_PRIME,
        r, l, u, d, f, b, r_PRIME, l_PRIME, u_PRIME, d_PRIME, f_PRIME, b_PRIME
    };

    int totalNodesVisited = 0;
    long long sumOfDepths = 0;
    int maxDepthEncountered = 0;
    int nodesRevisited = 0;

    while (!stack.empty()) {
        TreeNode* current = stack.top();
        stack.pop();

        totalNodesVisited++;

        if (current->get_cube().check_goal_state()) {
            std::cout << "[DFS Stats]\n";
            std::cout << "  Total nodes visited: " << totalNodesVisited << "\n";
            std::cout << "  Nodes skipped: " << nodesRevisited << "\n";
            std::cout << "  Average depth: " << (double) sumOfDepths / std::max(1, totalNodesVisited) << "\n";
            std::cout << "  Max depth reached: " << maxDepthEncountered << "\n";

            return current->get_solution_path();
        }

        int depth = 0;
        for (TreeNode* tmp = current; tmp->get_parent(); tmp = tmp->get_parent()) {
            depth++;
        }

        sumOfDepths += depth;
        if (depth > maxDepthEncountered) {
            maxDepthEncountered = depth;
        }

        if (depth >= depth_limit) {
            continue;
        }

        for (auto it = all_moves.rbegin(); it != all_moves.rend(); ++it) {
            Cube4x4 new_cube = current->get_cube();
            new_cube.move(*it);

            if (visited.find(new_cube) != visited.end()) {
                nodesRevisited++;
                continue;
            }
            visited.insert(new_cube);

            TreeNode* child = current->add_child(new_cube, *it);
            stack.push(child);
        }
    }

    std::cout << "[DFS Stats]\n";
    std::cout << "  Total nodes visited: " << totalNodesVisited << "\n";
    std::cout << "  Nodes skipped: " << nodesRevisited << "\n";
    std::cout << "  Average depth: " << (double) sumOfDepths / std::max(1, totalNodesVisited) << "\n";
    std::cout << "  Max depth reached: " << maxDepthEncountered << "\n";

    return {};
}



std::vector<Move> solve_cube_ids(const Cube4x4 &start_cube, int max_depth)
{
    for (int limit = 0; limit <= max_depth; limit++) {
        std::vector<Move> result = depth_limited_dfs_no_duplicates(start_cube, limit);
        if (!result.empty()) {
            return result;
        }
    }
    return {};
}


std::vector<Move> solve_any_phase_ida(const Cube4x4 &start_cube, int phase, std::vector<Move> all_moves, int maxDepth)
{

    double threshold = start_cube.twist_distance(phase);

    int totalVisited   = 0; 
    int duplicates     = 0; 
    int prunedByF      = 0; 

    while (true) {
        double next_threshold = LONG_MAX;

        std::stack<TreeNode*> st;
        TreeNode* root = new TreeNode(start_cube);
        st.push(root);

        std::unordered_set<Cube4x4, CubeHash, CubeEqual> visited;
        visited.insert(start_cube);

        bool foundSolution = false;
        TreeNode* goalNode = nullptr;


        while (!st.empty()) {
            TreeNode* current = st.top();
            st.pop();

            totalVisited++;

            const Cube4x4 &curCube = current->get_cube();
            
            double h = curCube.twist_distance(phase);
            if (h == 0.0) {
                foundSolution = true;
                goalNode = current;
                break;
            }

            int g = 0;
            for (TreeNode* tmp = current; tmp->get_parent(); tmp = tmp->get_parent()) {
                g++;
            }

            if (g >= maxDepth) {
                continue;
            }

            double f = g + h;
            if (f > threshold) {
                prunedByF++;
                if (f < next_threshold) {
                    next_threshold = f;
                }
                continue;
            }

            for (Move mv : all_moves) {
                Cube4x4 new_cube = curCube;
                new_cube.move(mv);

                if (visited.find(new_cube) != visited.end()) {
                    duplicates++;
                    continue;
                }
                visited.insert(new_cube);

                TreeNode* child = current->add_child(new_cube, mv);
                st.push(child);
            }
        } 

        if (foundSolution && goalNode) {
            std::cout << "[IDA* Stats]\n";
            std::cout << "  Max depth allowed : " << maxDepth << "\n";
            std::cout << "  Total visited     : " << totalVisited << "\n";
            std::cout << "  Duplicates skipped: " << duplicates << "\n";
            std::cout << "  Pruned by f       : " << prunedByF << "\n";
            return goalNode->get_solution_path();
        }

        if (next_threshold == LONG_MAX) {
            std::cout << "[IDA* Stats]\n";
            std::cout << "  Max depth allowed : " << maxDepth << "\n";
            std::cout << "  Total visited     : " << totalVisited << "\n";
            std::cout << "  Duplicates skipped: " << duplicates << "\n";
            std::cout << "  Pruned by f       : " << prunedByF << "\n";

            return {};
        }

        threshold = next_threshold;
    }
}


int oldmain() {
    Cube4x4 cube;
    std::vector<Move> scramble = cube.apply_random_moves(35, {});
    std::cout << "Scramble of size " << scramble.size() << " moves applied: ";
    for (Move move : scramble) {
        std::cout << move_to_string(move) << " ";
    }
    cube.print();
    
    std::cout << "\nSolving Cube...";

    int max_depth = 7;
    std::cout << "\n\nSolving phase 5... \n";
    std::vector<Move> solution5 = solve_any_phase_ida(cube, 5, { R, L, U, D, F, B }, max_depth);
    //print_apply_solution(cube, solution5);

    max_depth = 10;
    std::cout << "\n\nSolving phase 6... \n";
    std::vector<Move> solution6 = solve_any_phase_ida(cube, 6, { R, L, U2, D2, F, B }, max_depth);
    //print_apply_solution(cube, solution6);

    max_depth = 10;
    std::cout << "\n\nSolving phase 7... \n";
    std::vector<Move> solution7 = solve_any_phase_ida(cube, 7, { R2, L2, U2, D2, F, B }, max_depth);
    //print_apply_solution(cube, solution7);

    cube.print();

    return 0;
}
