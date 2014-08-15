#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include "tree.h"

#include <stdio.h>

/* tree.c - An implementation of a "voxel tree", a graph representing
   a position in a three-dimensional grid with links to all its
   neighbours.  */

/* The beginning and end of the linked list of voxel tree pools.  */

static int current_chunk_size = 256;

static voxel_pool* voxel_pool_source = NULL;

static voxel_pool* current_voxel_pool = NULL;

voxel_pool*
get_current_voxel_pool ()
{
  return (current_voxel_pool);
}

void
set_current_voxel_pool (voxel_pool* pool)
{
  current_voxel_pool = pool;
}

static voxel_pool* voxel_pool_end = NULL;

const int voxel_tree_byte_size = sizeof (voxel_tree) +
  (sizeof (voxel_tree*) * (BRANCH_NW + 1));

/* Allocate a new voxel pool.  */

voxel_pool*
new_voxel_pool ()
{
  printf("Allocating pool... ");
  voxel_pool* new_pool = (voxel_pool*) (malloc (sizeof (voxel_pool)));
  if (new_pool)
    {
      new_pool->begin =
	(voxel_tree*) malloc (voxel_tree_byte_size * current_chunk_size);
      if (new_pool->begin)
	{
	  new_pool->end = new_pool->begin +
	    (voxel_tree_byte_size * current_chunk_size);
	  new_pool->next = NULL;
	  if (voxel_pool_source)
	    {
	      new_pool->index = voxel_pool_end->index + 1;
	      voxel_pool_end->next = new_pool;
	      new_pool->prev = voxel_pool_end;
	      voxel_pool_end = new_pool;
	    }
	  else
	    {
	      new_pool->prev = NULL;
	      new_pool->index = 0;
	      voxel_pool_source = new_pool;
	      voxel_pool_end = voxel_pool_source;
	    }
	  printf("[OK]\n");
	  return (voxel_pool_end);
	}
      else
	{
	  free (new_pool);
	}
    }
  return (NULL);
}

/* Get a voxel pool from its index number, inner function.  */

voxel_pool*
get_pool_from_index_ (int index, voxel_pool* this)
{
  if (this)
    {
      if (index == this->index)
	{
	  return (this);
	}
      else
	{
	  return (get_pool_from_index_ (index, this->next));
	}
    }
  return (NULL);
}

/* Get a voxel pool from its index number, outer function.  */

voxel_pool*
get_pool_from_index (int index)
{
  if (voxel_pool_source)
    {
      return (get_pool_from_index_ (index, voxel_pool_source));
    }
  return (NULL);
}

/* Free a voxel pool.  
   We have three states we need to take into account: no voxel pool exists,
   one voxel pool exists and n voxel pools exist.  */

void
free_voxel_pool (voxel_pool* pool)
{
  if (voxel_pool_source)
    {
      if (voxel_pool_source == voxel_pool_end)
	{
	  /* One voxel pool exists.  */
	  free (pool);
	  voxel_pool_source = NULL;
	  set_current_voxel_pool (NULL);
	  voxel_pool_end = NULL;
	}
      else
	{
	  /* N voxel pools exist.  */
	  pool->prev->next = pool->next;
	  pool->next->prev = pool->prev;
	  if (get_current_voxel_pool () == pool)
	    set_current_voxel_pool (NULL);
	  free (pool);
	}
    }
}

voxel_tree*
new_voxel_tree_from_pool (int geometry, int material)
{
  if (current_voxel_pool && current_voxel_pool->end !=
      (current_voxel_pool->current + voxel_tree_byte_size))
    {
      current_voxel_pool->current += voxel_tree_byte_size;
    }
  else
    {
      voxel_pool* new_pool = new_voxel_pool();
      if (new_pool)
	{
	  current_voxel_pool = new_pool;
	}
      else
	{
	  return (NULL);
	}
    }
  voxel_tree* new_tree = current_voxel_pool->current;
  new_tree->geometry = geometry;
  new_tree->material = material;
  direction dir;
  for (dir = BRANCH_D; dir < BRANCH_NW; dir++)
    {
      new_tree->branches[dir] = NULL;
    }
  return (new_tree);
}

/* Allocate a voxel tree  */

voxel_tree *
new_voxel_tree (int geometry, int material)
{
  voxel_tree *tree = malloc (sizeof (voxel_tree));
  if (tree)
    {
      tree->branches = malloc (sizeof (voxel_tree*) * (BRANCH_NW + 1));
      if (tree->branches)
	{
	  direction dir;
	  for (dir = BRANCH_D; dir < BRANCH_NW; dir++)
	    {
	      tree->branches[dir] = NULL;
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

direction
mirror_tree (direction dir)
{
  int mirror = (dir + (BRANCH_NW / 2));
  if (mirror > BRANCH_U)
    return (mirror - BRANCH_NW);
  return (mirror);
}

/* Grow a voxel tree.  */

voxel_tree*
grow_voxel_tree (int geometry, int material,
		 direction dir, voxel_tree* tree)
{
  tree->branches[dir] = new_voxel_tree (geometry, material);
  voxel_tree *branch = tree->branches[dir];
  branch->geometry = geometry;
  branch->material = material;
  /* branch->branches[mirror_tree (dir)] = tree;  */
  return (branch);
}

voxel_tree*
grow_voxel_tree_from_pool (int geometry, int material,
			   direction dir, voxel_tree* tree)
{
  tree->branches[dir] = new_voxel_tree_from_pool (geometry, material);
  voxel_tree *branch = tree->branches[dir];
  branch->geometry = geometry;
  branch->material = material;
  return (branch);
}

/* Create a new voxel trace.  */

voxel_trace*
new_voxel_trace (voxel_tree* pointer)
{
  voxel_trace* address = (voxel_trace*) malloc (sizeof (voxel_trace));
  if (address)
    {
      address->node = pointer;
      address->next = NULL;
      return (address);
    }
  return (NULL);
}

/* Return the last node in a voxel trace.  */

voxel_trace*
end_of_voxel_trace (voxel_trace* trace)
{
  if (trace->next)
    return (end_of_voxel_trace (trace->next));
  return (trace);
}

/* Add an address to a voxel trace.  */

void
add_voxel_trace (voxel_trace* trace, voxel_tree* tree)
{
  voxel_trace* address = new_voxel_trace (tree);
  voxel_trace* trace_end = end_of_voxel_trace (trace);
  trace_end->next = address;
}

/* Check if an address is in a voxel trace.  */

bool
is_in_voxel_trace (voxel_trace* trace, voxel_tree* address)
{
  if (trace->node == address)
    return (true);
  if (!trace->next)
    return (false);
  return (is_in_voxel_trace (trace->next, address));
}

/* Free a single node of a voxel tree.  */

void
free_voxel_tree_node (voxel_tree* tree)
{
  free (tree->branches);
  free (tree);
}

/* Free a voxel trace.  
   Building the list first should ensure tail call elimination.  */

void
free_voxel_trace (voxel_trace* trace)
{
  voxel_trace* next_p = NULL;
  if (trace)
    {
      next_p = trace->next;
      free (trace);
    }
  else
    {
      return;
    }
  free_voxel_trace (next_p); /* Writing the function like this is obtuse
				but it makes the tail call unambiguous.  */
}

/* Free a voxel tree - inner function.  */

void
free_voxel_tree_ (voxel_trace* trace, voxel_tree* tree)
{
  if (tree && !is_in_voxel_trace (trace, tree))
    {
      add_voxel_trace (trace, tree);
      direction dir;
      for (dir = BRANCH_D; dir < BRANCH_NW; dir++)
	{
	    free_voxel_tree_ (trace, tree->branches[dir]);
	}
      free_voxel_tree_node (tree);
    }
}

/* Free a voxel tree - outer function.  */

void
free_voxel_tree (voxel_tree* tree)
{
  voxel_trace* trace = new_voxel_trace (NULL);
  free_voxel_tree_ (trace, tree);
  free_voxel_trace (trace);
}

/* Convert a point and a direction to a neighbouring point in 3d space.  */

point
branch_to_cube (point p, direction dir)
{
  switch (dir)
    {
    case BRANCH_D:
      --p.z;
      return (p);
      break;
    case BRANCH_LN:
      --p.z;
      --p.y;
      return (p);
      break;
    case BRANCH_LNE:
      --p.z;
      --p.y;
      ++p.x;
      return (p);
      break;
    case BRANCH_LE:
      --p.z;
      ++p.x;
      return (p);
      break;
    case BRANCH_LSE:
      --p.z;
      ++p.y;
      ++p.x;
      return (p);
      break;
    case BRANCH_LS:
      --p.z;
      ++p.y;
      return (p);
      break;
    case BRANCH_LSW:
      --p.z;
      ++p.y;
      --p.x;
      return (p);
      break;
    case BRANCH_LW:
      --p.z;
      --p.x;
      return (p);
      break;
    case BRANCH_LNW:
      --p.z;
      --p.x;
      --p.y;
      return (p);
      break;
    case BRANCH_N:
      --p.y;
      return (p);
      break;
    case BRANCH_NE:
      --p.y;
      ++p.x;
      return (p);
      break;
    case BRANCH_E:
      ++p.x;
      return (p);
      break;
    case BRANCH_SE:
      ++p.y;
      ++p.x;
      return (p);
      break;
    case BRANCH_S:
      ++p.y;
      return (p);
      break;
    case BRANCH_SW:
      ++p.y;
      --p.x;
      return (p);
      break;
    case BRANCH_W:
      --p.x;
      return (p);
      break;
    case BRANCH_NW:
      --p.x;
      --p.y;
      return (p);
      break;
    case BRANCH_U:
      ++p.z;
      return (p);
      break;
    case BRANCH_UN:
      ++p.z;
      --p.y;
      return (p);
      break;
    case BRANCH_UNE:
      ++p.z;
      --p.y;
      ++p.x;
      return (p);
      break;
    case BRANCH_UE:
      ++p.z;
      ++p.x;
      return (p);
      break;
    case BRANCH_USE:
      ++p.z;
      ++p.y;
      ++p.x;
      return (p);
      break;
    case BRANCH_US:
      ++p.z;
      ++p.y;
      return (p);
      break;
    case BRANCH_USW:
      ++p.z;
      ++p.y;
      --p.x;
      return (p);
      break;
    case BRANCH_UW:
      ++p.z;
      --p.x;
      return (p);
      break;
    case BRANCH_UNW:
      ++p.z;
      --p.x;
      --p.y;
      return (p);
      break;
    }
  return (p);
}

/* Check if a point is out of bounds for the purposes of array indexing.  */

bool
out_of_bounds (int height, int width, int depth, point p)
{
  if (0 > p.x || p.x >= width ||
      0 > p.y || p.y >= height ||
      0 > p.z || p.z >= depth)
    return (true);
  return (false);
}

/* Create a cubic voxel tree.  */

voxel_tree*
create_cubic_voxel_area (int height, int width, int depth,
			 int geometry, int material)
{
  voxel_tree* area[width][height][depth];
  voxel_tree* this;
  voxel_tree* neighbour;
  point p, n_p;
  direction dir;
  for (p.x = 0; p.x < width; p.x++)
    {
      for (p.y = 0; p.y < height; p.y++)
	{
	  for (p.z = 0; p.z < depth; p.z++)
	    {
	      area[p.x][p.y][p.z] =
		new_voxel_tree(geometry, material);
	    }
	}
    }
  for (p.x = 0; p.x < width; p.x++)
    {
      for (p.y = 0; p.y < height; p.y++)
	{
	  for (p.z = 0; p.z < depth; p.z++)
	    {
	      for (dir = BRANCH_D; dir < BRANCH_NW; dir++)
		{
		  n_p = branch_to_cube(p, dir);
		  if (!out_of_bounds (height, width, depth, n_p))
		    {
		      neighbour = area[n_p.x][n_p.y][n_p.z];
		      this = area[p.x][p.y][p.z];
		      this->branches[dir] = neighbour;
		    }
		}
	    }
	}
    }
  return(area[0][0][0]);
}
