#ifndef TREE_H
#define TREE_H

#include <iostream>
#include <vector>
#include <queue>
#include "cube.h"

class TreeNode {
public:
    TreeNode(Cube4x4 cube_state, TreeNode* parent_cube = nullptr);

    TreeNode* add_child(Cube4x4 child_cube);

    void dfs(int depth = 0);

    void bfs();

private:
    Cube4x4 cube;
    TreeNode* parent;
    std::vector<TreeNode*> children;
};

#endif