static char level[80][24][20];

static char substance[80][24][20];

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
