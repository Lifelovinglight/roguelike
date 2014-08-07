#include <stdlib.h>
#include <stddef.h>

/* tree.c - An implementation of a "voxel tree", a graph representing
   a position in a three-dimensional grid with links to all its
   neighbours.  */

/* The branches of a voxel tree.  */

enum voxel_tree_branches
{
  BRANCH_D,
  BRANCH_LN,
  BRANCH_LNE,
  BRANCH_LE,
  BRANCH_LSE,
  BRANCH_LS,
  BRANCH_LSW,
  BRANCH_LW,
  BRANCH_LNW,
  BRANCH_N,
  BRANCH_NE,
  BRANCH_E,
  BRANCH_SE,
  BRANCH_S,
  BRANCH_SW,
  BRANCH_W,
  BRANCH_NW,
  BRANCH_UN,
  BRANCH_UNE,
  BRANCH_UE,
  BRANCH_USE,
  BRANCH_US,
  BRANCH_USW,
  BRANCH_UW,
  BRANCH_UNW,
  BRANCH_U
};

/* A voxel tree.  */

struct voxel_tree
{
  int geometry;
  int material;
  struct voxel_tree **branches;
};

/* Allocate a voxel tree  */

struct voxel_tree *
new_voxel_tree (int geometry, int material)
{
  struct voxel_tree *tree = malloc (sizeof (struct voxel_tree));
  if (NULL != tree)
    {
      tree->branches = malloc (sizeof (struct voxel_tree *) * (BRANCH_U + 1));
      if (NULL != tree->branches)
	{
	  int direction;
	  for (direction = BRANCH_D; direction <= BRANCH_U; ++direction)
	    {
	      tree->branches[direction] = NULL;
	    }
	  tree->geometry = geometry;
	  tree->material = material;
	  return (tree);
	}
      else
	{
	  free (tree);
	}
    }
  return (NULL);
}

/* Mirror the direction of a branch.  
   I.e. "mirror_tree (BRANCH_N)" returns "BRANCH_S".  */

int
mirror_tree (int direction)
{
  return (direction + BRANCH_U);
}

/* Grow a voxel tree.  */

struct voxel_tree *
grow_voxel_tree (int geometry, int material,
		 int direction, struct voxel_tree *tree)
{
  tree->branches[direction] = new_voxel_tree (geometry, material);
  struct voxel_tree *branch = tree->branches[direction];
  branch->geometry = geometry;
  branch->material = material;
  /* branch->branches[mirror_tree (direction)] = tree;  */
  return (branch);
}

/* Recursively free a voxel tree.  */

void
free_voxel_tree (struct voxel_tree *tree)
{
  int direction;
  for (direction = BRANCH_D; direction <= BRANCH_U; ++direction)
    {
      if (NULL != tree->branches[direction])
	free_voxel_tree (tree->branches[direction]);
    }
  free (tree->branches);
  free (tree);
}
