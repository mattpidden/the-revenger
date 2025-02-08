#include <algorithm>
#include <optional>

#include "tree.h"
#include "cube.h"

TreeNode::TreeNode(Cube4x4 cube_state, std::optional<Move> last_move, TreeNode* parent_cube) : cube(cube_state), move(last_move), parent(parent_cube) {}

TreeNode* TreeNode::add_child(Cube4x4 child_cube, Move next_move) {
    TreeNode* child = new TreeNode(child_cube, next_move, this);
    children.push_back(child);
    return child;
}

std::vector<Move> TreeNode::get_solution_path() const {
    std::vector<Move> moves;
    const TreeNode* current = this;
    while (current->parent != nullptr && current->move.has_value()) {  // ✅ Ensure move is valid
        moves.push_back(current->move.value());
        current = current->parent;
    }
    std::reverse(moves.begin(), moves.end());
    return moves;
}