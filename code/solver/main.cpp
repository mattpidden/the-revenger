#include "cube.h"
#include "tree.h"


int main() {
  Cube4x4 cube;

  std::vector<Move> scramble = cube.apply_random_moves(5);

  cube.print();

  TreeNode* root = new TreeNode(cube);
  TreeNode* n2 = root->add_child(cube);
  TreeNode* n3 = root->add_child(cube);

  std::cout << "DFS Traversal:\n";
  root->dfs();

  std::cout << "\nBFS Traversal:\n";
  root->bfs();



  return 0;
}