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

    // Stats
    int totalNodesVisited = 0;
    long long sumOfDepths = 0;  // to calculate average depth
    int maxDepthEncountered = 0;
    int nodesRevisited = 0;

    while (!stack.empty()) {
        TreeNode* current = stack.top();
        stack.pop();

        // Count the node as "visited"
        totalNodesVisited++;

        // Check goal
        if (current->get_cube().check_goal_state()) {
            // Print stats before returning
            std::cout << "[DFS Stats]\n";
            std::cout << "  Total nodes visited: " << totalNodesVisited << "\n";
            std::cout << "  Average depth: "
                      << (double) sumOfDepths / std::max(1, totalNodesVisited)
                      << "\n";
            std::cout << "  Max depth reached: " << maxDepthEncountered << "\n";
            // Return solution
            return current->get_solution_path();
        }

        // Determine depth by following parents
        int depth = 0;
        for (TreeNode* tmp = current; tmp->get_parent(); tmp = tmp->get_parent()) {
            depth++;
        }
        // Update stats
        sumOfDepths += depth;
        if (depth > maxDepthEncountered) {
            maxDepthEncountered = depth;
        }

        // If we are at the limit, don't expand further
        if (depth >= depth_limit) {
            continue;
        }

        // Expand children (reverse iteration to preserve order on stack)
        for (auto it = all_moves.rbegin(); it != all_moves.rend(); ++it) {
            Cube4x4 new_cube = current->get_cube();
            new_cube.move(*it);

            // Skip if we’ve already seen this cube
            if (visited.find(new_cube) != visited.end()) {
                nodesRevisited++;
                continue;
            }
            visited.insert(new_cube);

            TreeNode* child = current->add_child(new_cube, *it);
            stack.push(child);
        }
    }

    // If we exhaust the stack without finding a solution, we still print stats
    std::cout << "[DFS Stats]\n";
    std::cout << "  Total nodes visited: " << totalNodesVisited << "\n";
    std::cout << "  Nodes skipped: " << nodesRevisited << "\n";

    std::cout << "  Average depth: " 
              << (double) sumOfDepths / std::max(1, totalNodesVisited)
              << "\n";
    std::cout << "  Max depth reached: " << maxDepthEncountered << "\n";

    // Return empty if no solution found
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


int main() {
    Cube4x4 cube;
  
    std::vector<Move> scramble = cube.apply_random_moves(5);
    std::cout << "Scramble of size " << scramble.size() << " moves applied: ";
    for (Move move : scramble) {
        std::cout << move_to_string(move) << " ";
    }

    cube.print();

    int max_depth = 5;
    std::cout << "\nSolving Cube... \n";
    std::vector<Move> solution = depth_limited_dfs_no_duplicates(cube, 5);

    if (!solution.empty()) {
        std::cout << "Solution found in " << solution.size() << " moves: ";
        for (Move move : solution) {
            std::cout << move_to_string(move) << " ";
            cube.move(move);
        }
        cube.print();
    } else {
        std::cout << "No solution found within " << max_depth << " moves.\n";
    }

    return 0;
}
