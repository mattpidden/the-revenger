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


/**
 * @brief IDA* for Phase 1, using phase_one_twist_distance() as the heuristic.
 *        We consider the puzzle “in goal” if phase_one_twist_distance() < 1.
 */
std::vector<Move> solve_phase_one_ida(const Cube4x4 &start_cube)
{
    // All moves allowed in Phase 1
    std::vector<Move> all_moves = {
        R, L, U, D, F, B, R_PRIME, L_PRIME, U_PRIME, D_PRIME, F_PRIME, B_PRIME,
        r, l, u, d, f, b, r_PRIME, l_PRIME, u_PRIME, d_PRIME, f_PRIME, b_PRIME
    };

    // Initial threshold = heuristic of start state
    int threshold = start_cube.phase_one_twist_distance();

    // We'll do iterative deepening based on f = g + h
    while (true) {
        int next_threshold = INT_MAX;

        // Use a stack for DFS expansions
        std::stack<TreeNode*> st;
        TreeNode *root = new TreeNode(start_cube);
        st.push(root);

        // Visited set to avoid duplicate states in the same iteration
        std::unordered_set<Cube4x4, CubeHash, CubeEqual> visited;
        visited.insert(start_cube);

        bool foundSolution = false;
        TreeNode *goalNode = nullptr;

        // Depth-first search with cost limit
        while (!st.empty()) {
            TreeNode* current = st.top();
            st.pop();

            const Cube4x4 &curCube = current->get_cube();

            // Phase 1 goal check
            // If twist_distance < 1, we treat it as solved for Phase 1
            if (curCube.phase_one_twist_distance() == 0) {
                foundSolution = true;
                goalNode = current;
                break;
            }

            // g = depth
            int g = 0;
            for (TreeNode *tmp = current; tmp->get_parent(); tmp = tmp->get_parent()) {
                g++;
            }

            // h = phase_one_twist_distance
            int h = curCube.phase_one_twist_distance();
            int f = g + h;

            // If cost exceeds the current threshold, skip and record f
            if (f > threshold) {
                next_threshold = std::min(next_threshold, f);
                continue;
            }

            // Expand children
            for (Move mv : all_moves) {
                Cube4x4 new_cube = curCube;
                new_cube.move(mv);

                if (visited.find(new_cube) != visited.end()) {
                    continue; // Skip duplicates
                }
                visited.insert(new_cube);

                TreeNode* child = current->add_child(new_cube, mv);
                st.push(child);
            }
        }

        if (foundSolution && goalNode) {
            return goalNode->get_solution_path();
        }

        // If we never updated next_threshold => no expansions => no solution
        if (next_threshold == INT_MAX) {
            return {}; // Nothing found
        }

        // Raise threshold & try again
        threshold = next_threshold;
    }
}


int main() {
    Cube4x4 cube;
    

    std::vector<Move> scramble = cube.apply_random_moves(35);
    std::cout << "Scramble of size " << scramble.size() << " moves applied: ";
    for (Move move : scramble) {
        std::cout << move_to_string(move) << " ";
    }

    cube.print();
    int h = cube.phase_one_twist_distance();
    
    std::cout << "Phase 1 Heuristic: " << h << "\n";
    //int max_depth = 5;
    std::cout << "\nSolving Cube... \n";
    
    std::vector<Move> solution = solve_phase_one_ida(cube);

    if (!solution.empty()) {
        std::cout << "Solution found in " << solution.size() << " moves: ";
        for (Move move : solution) {
            std::cout << move_to_string(move) << " ";
            cube.move(move);
        }
        cube.print();
    } else {
        std::cout << "No solution found. \n";
    }
    
    

    return 0;
}
