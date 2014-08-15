/* The branches of a voxel tree.  */

typedef enum
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
  BRANCH_U,
  BRANCH_US,
  BRANCH_USW,
  BRANCH_UW,
  BRANCH_UNW,
  BRANCH_UN,
  BRANCH_UNE,
  BRANCH_UE,
  BRANCH_USE,
  BRANCH_S,
  BRANCH_SW,
  BRANCH_W,
  BRANCH_NW
} direction;

/* A voxel tree.  */

typedef struct voxel_tree voxel_tree;

struct voxel_tree
{
  int geometry;
  int material;
  voxel_tree** branches;
};

/* A linked list of voxel tree pointers.  
   Used to avoid problems with cyclic graphs when freeing a voxel tree.  */

typedef struct voxel_trace voxel_trace;

struct voxel_trace
{
  voxel_tree* node;
  voxel_trace* next;
};

/* A memory pool for voxel trees.  */

typedef struct voxel_pool voxel_pool;

struct voxel_pool
{
  voxel_tree* begin;
  voxel_tree* current;
  voxel_tree* end;
  int index;
  voxel_pool* prev;
  voxel_pool* next;
};

/* A point in 3d space.  */

typedef struct
{
  int x;
  int y;
  int z;
} point;

voxel_tree*
new_voxel_tree (int geometry, int material);

voxel_tree*
grow_voxel_tree (int geometry, int material,
		 direction dir, struct voxel_tree* tree);

direction
mirror_tree(direction dir);

void
free_voxel_tree (voxel_tree* tree);

voxel_tree*
create_cubic_voxel_area (int height, int width, int depth,
			 int geometry, int material);

voxel_pool*
new_voxel_pool ();

voxel_tree*
grow_voxel_tree_from_pool (int geometry, int material,
			   direction dir, voxel_tree* tree);

voxel_pool*
get_pool_from_index (int index);

void
free_voxel_pool (voxel_pool* pool);

voxel_tree*
new_voxel_tree_from_pool (int geometry, int material);

voxel_pool*
get_current_voxel_pool ();

void
set_current_voxel_pool (voxel_pool* pool);
