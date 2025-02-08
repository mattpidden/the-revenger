#include "tree.h"
#include "cube.h"

TreeNode::TreeNode(Cube4x4 cube_state, TreeNode* parent_cube) : cube(cube_state), parent(parent_cube) {}

TreeNode* TreeNode::add_child(Cube4x4 child_cube) {
    TreeNode* child = new TreeNode(child_cube, this);
    children.push_back(child);
    return child;
}

void TreeNode::dfs(int depth) {
    for (TreeNode* child : children) {
        child->dfs(depth + 1);  
    }
}

void TreeNode::bfs() {
    std::queue<TreeNode*> q;
    q.push(this);

    while (!q.empty()) {
        TreeNode* current = q.front();
        q.pop();

        for (TreeNode* child : current->children) {
            q.push(child);
        }
    }
    std::cout << "\n";
}
