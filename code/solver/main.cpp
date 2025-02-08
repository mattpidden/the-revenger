#include "tree.h"
#include <queue>

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

int main() {
    Cube4x4 cube;
    std::vector<Move> scramble = cube.apply_random_moves(4);
    std::cout << "Scramble of size " << scramble.size() << " moves applied: ";
    for (Move move : scramble) {
        std::cout << move_to_string(move) << " ";
    }

    cube.print();

    std::cout << "\nSolving Cube... \n";
    std::vector<Move> solution = solve_cube_bfs(cube, 5);

    if (!solution.empty()) {
        std::cout << "Solution found in " << solution.size() << " moves: ";
        for (Move move : solution) {
            std::cout << move_to_string(move) << " ";
        }
        std::cout << "\n";
        cube.print();
    } else {
        std::cout << "No solution found within 3 moves.\n";
    }

    return 0;
}
