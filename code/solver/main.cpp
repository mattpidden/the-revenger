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

std::vector<Move> solve_cube_dfs(Cube4x4 start_cube, int max_depth) {
    std::stack<TreeNode*> stack;
    TreeNode* root = new TreeNode(start_cube);
    stack.push(root);

    std::vector<Move> all_moves = {
        R, L, U, D, F, B, R_PRIME, L_PRIME, U_PRIME, D_PRIME, F_PRIME, B_PRIME,
        r, l, u, d, f, b, r_PRIME, l_PRIME, u_PRIME, d_PRIME, f_PRIME, b_PRIME
    };

    while (!stack.empty()) {
        TreeNode* current = stack.top();
        stack.pop();

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

        for (auto it = all_moves.rbegin(); it != all_moves.rend(); ++it) {
            Cube4x4 new_cube = current->get_cube();
            new_cube.move(*it);
            TreeNode* child = current->add_child(new_cube, *it);
            stack.push(child);
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


std::vector<Move> ida_star_search(Cube4x4 start_cube)
{
    std::vector<Move> all_moves = {
        R, L, U, D, F, B, R_PRIME, L_PRIME, U_PRIME, D_PRIME, F_PRIME, B_PRIME,
        r, l, u, d, f, b, r_PRIME, l_PRIME, u_PRIME, d_PRIME, f_PRIME, b_PRIME
    };


    int threshold = start_cube.misplaced_pieces_heuristic();

    while (true) {
        int next_threshold = INT_MAX;

        std::unordered_set<Cube4x4, CubeHash, CubeEqual> visited;
        
        TreeNode* root = new TreeNode(start_cube);
        std::stack<TreeNode*> st;
        st.push(root);

        bool foundSolution = false;
        TreeNode* goalNode = nullptr;

        while (!st.empty()) {
            TreeNode* current = st.top();
            st.pop();

            const Cube4x4 &curCube = current->get_cube();
            visited.insert(curCube);

            if (curCube.check_goal_state()) {
                foundSolution = true;
                goalNode = current;
                break;
            }

            int g = 0;
            for (TreeNode* tmp = current; tmp->get_parent(); tmp = tmp->get_parent()) {
                g++;
            }

            int h = curCube.misplaced_pieces_heuristic();
            int f = g + h;

            if (f > threshold) {
                next_threshold = std::min(next_threshold, f);
                continue;
            }

            for (Move mv : all_moves) {
                Cube4x4 new_cube = curCube;
                new_cube.move(mv);

                if (visited.find(new_cube) != visited.end()) {
                    continue;
                }
                TreeNode* child = current->add_child(new_cube, mv);
                st.push(child);
            }
        }

        if (foundSolution && goalNode) {
            return goalNode->get_solution_path();
        }
        if (next_threshold == INT_MAX) {
            return {};
        }

        threshold = next_threshold;
    }
}


int main() {
    Cube4x4 cube;
  
    std::vector<Move> scramble = cube.apply_random_moves(3);
    std::cout << "Scramble of size " << scramble.size() << " moves applied: ";
    for (Move move : scramble) {
        std::cout << move_to_string(move) << " ";
    }

    cube.print();

    int max_depth = 5;
    std::cout << "\nSolving Cube... \n";
    std::vector<Move> solution = solve_cube_dfs(cube, 5);

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
