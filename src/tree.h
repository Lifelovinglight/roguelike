struct voxel_tree
{
  int geometry;
  int material;
  struct voxel_tree** branches;
};


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

struct voxel_tree* new_voxel_tree (int geometry, int material);

struct voxel_tree* grow_voxel_tree (int geometry, int material,
				    int direction, struct voxel_tree* tree);

enum voxel_tree_branches;

int mirror_tree(int direction);

struct voxel_tree* free_voxel_tree (struct voxel_tree* tree);

struct voxel_tree* create_cubic_voxel_area (int height, int width, int depth,
					    int geometry, int material);
