#ifndef TREE_H
#define TREE_H

#include <iostream>
#include <vector>
#include <queue>
#include <optional>

#include "cube.h"

class TreeNode {
public:
    TreeNode(Cube4x4 cube_state, std::optional<Move> last_move = std::nullopt, TreeNode* parent_cube = nullptr);

    TreeNode* add_child(Cube4x4 child_cube, Move next_move);

    std::vector<Move> get_solution_path() const;

    TreeNode* get_parent() const { return parent; }

    Cube4x4 get_cube() const { return cube; }

private:
    Cube4x4 cube;
    std::optional<Move> move;
    TreeNode* parent;
    std::vector<TreeNode*> children;
};

#endif 
