#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "tree.h"

int
main (int argc, char *argv[])
{
  struct voxel_tree* tree = new_voxel_tree (argc, argc);
  int n = tree->geometry;
  grow_voxel_tree (argc, (argc + 1), BRANCH_U, tree);
  int i = tree->branches[BRANCH_U]->material;
  printf ("%d, %d\n", n, i);
  free_voxel_tree (tree);
  return(0);
}
