static char level[80][24][20];

static char substance[80][24][20];

/*

struct voxel_tree
{
  char geometry;
  char material;
  struct voxel_tree* d;
  struct voxel_tree* ln;
  struct voxel_tree* lne;
  struct voxel_tree* le;
  struct voxel_tree* lse;
  struct voxel_tree* ls;
  struct voxel_tree* lsw;
  struct voxel_tree* lw;
  struct voxel_tree* lnw;
  struct voxel_tree* n;
  struct voxel_tree* ne;
  struct voxel_tree* e;
  struct voxel_tree* se;
  struct voxel_tree* s;
  struct voxel_tree* sw;
  struct voxel_tree* w;
  struct voxel_tree* nw;
  struct voxel_tree* un;
  struct voxel_tree* une;
  struct voxel_tree* ue;
  struct voxel_tree* use;
  struct voxel_tree* us;
  struct voxel_tree* usw;
  struct voxel_tree* uw;
  struct voxel_tree* unw;
  struct voxel_tree* u;
};

static voxel_tree* player_pointer;

*/

struct xyz
{
  int x;
  int y;
  int z;
};

typedef enum
  {
    AIR,
    WALL,
    SLOPE,
    DOOR_CLOSED,
    DOOR_OPEN,
    LADDER
  } terrains;

typedef enum
  {
    NOTHING,
    SOIL,
    GRASS,
    ROCK,
    WOOD,
    CONCRETE,
    STEEL
  } materials;

typedef enum
  {
    WHITE,
    BLACK,
    RED,
    GREEN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN
  } colors;

#define TRUE 1
#define FALSE 0
