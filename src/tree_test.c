/* tree_test.c - test the voxel tree implementation.  */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "tree.h"

/* Return a random tree branch. */

int
random_tree_branch ()
{
  return (random () % (BRANCH_U + 1));
}

/* Returns the first available branch.  
   if all branches are NULL, return NULL.  */

struct voxel_tree*
first_legal_voxel_path (struct voxel_tree* tree)
{
  int direction;
  for (direction = BRANCH_D; direction <= BRANCH_U; ++direction)
    {
      if (NULL != tree->branches[direction])
	return(tree->branches[direction]);
    }
  return(NULL);
}

/* Return a random single-digit number.  */

int rsd ()
{
  return(random () % 10);
}

int
main (int argc, char *argv[])
{
  struct voxel_tree* tree = new_voxel_tree (rsd (), rsd ());
  struct voxel_tree* node = tree;
  int n;
  for (n = 0; n < 2; ++n)
    {
      node = grow_voxel_tree (rsd (), rsd (), random_tree_branch (), node);
    }
  printf ("\n\n");
  struct voxel_tree* t;
  for (t = tree; NULL != t;)
    {
      printf("%d %d ", t->geometry, t->material);
      t = first_legal_voxel_path (t);
    }
  printf ("\n\n");
  node->branches[BRANCH_U] = tree;
  free_voxel_tree (tree);
  tree = create_cubic_voxel_area (3, 3, 3, 1, 1);
  free_voxel_tree (tree);
  return (0);
}
