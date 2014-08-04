#include <stdlib.h>
#include <ncurses.h>
#include <libguile.h>
#include "curses.h"

int
one_in(int n)
{
  return(random() % (n + 1));
}

int
out_of_bounds(struct xyz point)
{
  if(point.x > 79 || point.x < 0 ||
     point.y > 23 || point.y < 0 ||
     point.z > 19 || point.z < 0)
    {
      return(TRUE);
    }
  else
    {
      return(FALSE);
    }
}

void
set_terrain(struct xyz point, char c)
{
  if(!out_of_bounds(point))
    {
      level[point.x][point.y][point.z] = c;
    }
}

void
set_material(struct xyz point, char c)
{
  substance[point.x][point.y][point.z] = c;
}

static struct xyz
player = {10,10,10};

struct xyz
north(struct xyz point)
{
  point.y -= 1;
  return(point);
}

struct xyz
east(struct xyz point)
{
  point.x += 1;
  return(point);
}

struct xyz
south(struct xyz point)
{
  point.y += 1;
  return(point);
}

struct xyz
west(struct xyz point)
{
  point.x -= 1;
  return(point);
}

struct xyz
down(struct xyz point)
{
  point.z -= 1;
  return(point);
}

struct xyz
up(struct xyz point)
{
  point.z += 1;
  return(point);
}

struct xyz
directions2d(struct xyz point, char n)
{
  switch(n)
    {
    case 0:
      return(north(point));
      break;
    case 1:
      return(north(east(point)));
      break;
    case 2:
      return(east(point));
      break;
    case 3:
      return(south(east(point)));
      break;
    case 4:
      return(south(point));
      break;
    case 5:
      return(south(west(point)));
      break;
    case 6:
      return(west(point));
      break;
    case 7:
      return(north(west(point)));
      break;
    }
  return(point);
}

void
init_level()
{
  struct xyz point;
  for(point.x = 0; point.x < 80; point.x++)
    {
      for(point.y = 0; point.y < 24; point.y++)
	{
	  for(point.z = 0; point.z < 20; point.z++)
	    {
	      if(!out_of_bounds(point))
		{
		  if(point.z < 10)
		    {
		      set_terrain(point, WALL);
		      if(point.z == 9)
			{
			  set_material(point, GRASS);
			}
		      else
			{
			  if(point.z > 7)
			    {
			      set_material(point, SOIL);
			    }
			  else
			    {
			      set_material(point, ROCK);
			    }
			}
		    }
		  else
		    {
		      set_terrain(point,AIR);
		    }
		}
	    }
	}	  
    }
}

void
render(struct xyz point, char c, char color, int intensity)
{
  switch(intensity)
    {
    case 2:
      attrset(A_BOLD | COLOR_PAIR(color));
      break;
    case 1:
      attrset(COLOR_PAIR(color));
      break;
    case 0:
      attrset(A_BOLD | COLOR_PAIR(BLACK));
      break;
    }
  mvaddch(point.y,point.x,c);
}

char
terrain(struct xyz point)
{
  if(out_of_bounds(point))
    {
      return(WALL);
    }
  else
    {
      return(level[point.x][point.y][point.z]);
    }
}

char
material(struct xyz point)
{
  if(out_of_bounds(point))
    {
      return(NOTHING);
    }
  else
    {
      return(substance[point.x][point.y][point.z]);
    }
  return(NOTHING);
}

void
transform_terrain(struct xyz point, char x, char y)
{
  if(terrain(point) == x)
    set_terrain(point,y);
}
 
int
surrounded_by_walls(struct xyz point)
{
  char n;
  struct xyz d;
  char c = material(point);
  for(n = 0; n < 8; n++)
    {
      d = directions2d(point, n);
      if(terrain(d) != WALL || (material(d) != c && material(d) != NOTHING))
	return(FALSE);
    }
  return(TRUE);	  
}
		 
char
glyph(struct xyz point, char above)
{ 
  switch(terrain(point))
    {
    case WALL:
      if(above == FALSE && surrounded_by_walls(point))
	{
	  return(' ');
	}
      else
	{
	  return(above ? '.' : '#');
	}
      break;
    case SLOPE:
      return('^');
      break;
    case DOOR_CLOSED:
      return('+');
      break;
    case DOOR_OPEN:
      return(',');
      break;
    case LADDER:
      return(':');
      break;
    }
  return(' ');
}

char
color(struct xyz point, char above)
{
  switch(material(point))
    {
    case SOIL:
      return(YELLOW);
      break;
    case ROCK:
      return(WHITE);
      break;
    case GRASS:
      return(above ? GREEN : YELLOW);
      break;
    case WOOD:
      return(YELLOW);
      break;
    case CONCRETE:
      return(WHITE);
      break;
    case STEEL:
      return(WHITE);
      break;
    }
  return(NOTHING);
}

void
render_column(struct xyz point, int intensity)
{
  if(intensity >= 0)
    {
      if(terrain(point) == AIR && terrain(down(point)) == WALL)
	{
	  render(point, glyph(down(point), TRUE), color(down(point), TRUE), intensity);
	  return;
	}
      if(terrain(point) == AIR)
	{
	  render_column(down(point), --intensity);
	}
      else
	{
	  render(point, glyph(point, intensity < 2 ? TRUE : FALSE),
		 color(point, intensity < 2 ? TRUE : FALSE), intensity);
	}
    }
}

void
draw_level(int z)
{
  struct xyz point;
  point.z = z;
  for(point.x = 0; point.x < 80; point.x++)
    {
      for(point.y = 0; point.y < 24; point.y++)
	{
	  if(!out_of_bounds(point))
	    {
	      render_column(point,2);
	    }
	}
    }
}

/*

void
line_of_sight(struct xyz start, struct xyz dest)
{
 
  int dx = abs(dest.x - start.x), sx = start.x < dest.x ? 1 : -1;
  int dy = abs(dest.y - start.y), sy = start.y < dest.y ? 1 : -1; 
  int err = (dx > dy ? dx : -dy)/2, e2;
 
  for(;;){
    vision[start.x][start.y] = 1;
    if (!passable(start)) break;
    if (start.x == dest.x && start.y == dest.y) break;
    e2 = err;
    if (e2 >-dx) { err -= dy; start.x += sx; }
    if (e2 < dy) { err += dx; start.x += sy; }
  }
}

void
calculate_vision(struct zyx point)
{
  int n;
  struct xyz dest;
  for(x = 0; x < 80; x++)
    {
      for(y = 0; y < 24; y++)
	{
	  dest.x = 0;
	  dest.y = n;
	  line_of_sight(point, dest);
	}
    }
}

*/

char
num_ng_mat(struct xyz point, char c)
{
  char n = 0;
  if(material(down(point)) == c) ++n;
  if(material(down(north(point))) == c) ++n;
  if(material(down(north(east(point)))) == c) ++n;
  if(material(down(east(point))) == c) ++n;
  if(material(down(south(east(point)))) == c) ++n;
  if(material(down(south(point))) == c) ++n;
  if(material(down(south(west(point)))) == c) ++n;
  if(material(down(west(point))) == c) ++n;
  if(material(down(north(west(point)))) == c) ++n;
  if(material(north(point)) == c) ++n;
  if(material(north(east(point))) == c) ++n;
  if(material(east(point)) == c) ++n;
  if(material(south(east(point))) == c) ++n;
  if(material(south(point)) == c) ++n;
  if(material(south(west(point))) == c) ++n;
  if(material(west(point)) == c) ++n;
  if(material(north(west(point))) == c) ++n;
  if(material(up(north(point))) == c) ++n;
  if(material(up(north(east(point)))) == c) ++n;
  if(material(up(east(point))) == c) ++n;
  if(material(up(south(east(point)))) == c) ++n;
  if(material(up(south(point))) == c) ++n;
  if(material(up(south(west(point)))) == c) ++n;
  if(material(up(west(point))) == c) ++n;
  if(material(up(north(west(point)))) == c) ++n;
  if(material(up(point)) == c) ++n;
  return(n);
}

/*

void
grow_grass()
{
  struct xyz n;
  for(n.x = 0; n.x < 80; n.x++)
    {
      for(n.y = 0; n.y < 80; n.y++)
	{
	  for(n.z = 0; n.z < 20; n.z++)
	    {
	      if(!out_of_bounds(n))
		{
		  if(material(n) == SOIL && is_outside(up(n)) && num_ng_mat(n, GRASS) &&
		     random() % 100 == 0)
		    {
		      set_material(n, GRASS);
		    }
		}
	    }
	}
    }
}

*/

SCM
scm_from_xyz(struct xyz point)
{
  SCM x = scm_from_int(point.x);
  SCM y = scm_from_int(point.y);
  SCM z = scm_from_int(point.z);
  return(scm_list_3(x, y, z));
}

struct xyz
scm_to_xyz(SCM point)
{
  struct xyz p;
  p.x = scm_to_int(scm_list_ref(point, scm_from_int(0)));
  p.y = scm_to_int(scm_list_ref(point, scm_from_int(1)));
  p.z = scm_to_int(scm_list_ref(point, scm_from_int(2)));
  return(p);
}

static SCM
key_right(void)
{
  return(scm_from_int(KEY_RIGHT));
}

static SCM key_left(void)
{
  return(scm_from_int(KEY_LEFT));
}

static SCM
key_up(void)
{
  return(scm_from_int(KEY_UP));
}

static SCM
key_down(void)
{
  return(scm_from_int(KEY_DOWN));
}

SCM
scm_terrain(SCM point)
{
  return(scm_from_char(terrain(scm_to_xyz(point))));
}

SCM
scm_material(SCM point)
{
  return(scm_from_char(material(scm_to_xyz(point))));
}

SCM
scm_set_terrain(SCM point, SCM c)
{
  set_terrain(scm_to_xyz(point), scm_to_char(c));
  return(scm_from_int(0));
}

SCM
scm_set_material(SCM point, SCM m)
{
  set_material(scm_to_xyz(point), scm_to_char(m));
  return(scm_from_int(0));
}

SCM
scm_out_of_bounds(SCM point)
{
  if(out_of_bounds(scm_to_xyz(point)))
    {
      return(SCM_BOOL_T);
    }
  else
    {
      return(SCM_BOOL_F);
    }
  return(SCM_BOOL_T);
}

SCM
scm_getch(void)
{
  return(scm_from_int(getch()));
}

int
main(int argc, char *argv[])
{
  char exit = FALSE;
  initscr();
  start_color();
  init_pair(WHITE, COLOR_WHITE, COLOR_BLACK);
  init_pair(BLACK, COLOR_BLACK, COLOR_BLACK);
  init_pair(RED, COLOR_RED, COLOR_BLACK);
  init_pair(GREEN, COLOR_GREEN, COLOR_BLACK);
  init_pair(YELLOW, COLOR_YELLOW, COLOR_BLACK);
  init_pair(BLUE, COLOR_BLUE, COLOR_BLACK);
  init_pair(MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(CYAN, COLOR_CYAN, COLOR_BLACK);
  cbreak();
  noecho();
  nonl();
  intrflush(stdscr, FALSE);
  keypad(stdscr, TRUE);
  curs_set(0);
  srand(42);
  scm_init_guile();
  scm_c_define_gsubr("key_right", 0, 0, 0, key_right);
  scm_c_define_gsubr("key_left", 0, 0, 0, key_left);
  scm_c_define_gsubr("key_up", 0, 0, 0, key_up);
  scm_c_define_gsubr("key_down", 0, 0, 0, key_down);
  scm_c_define_gsubr("set-terrain", 2, 0, 0, scm_set_terrain);
  scm_c_define_gsubr("terrain", 1, 0, 0, scm_terrain);
  scm_c_define_gsubr("set-material", 2, 0, 0, scm_set_material);
  scm_c_define_gsubr("material", 1, 0, 0, scm_material);
  scm_c_define_gsubr("out-of-bounds", 1, 0, 0, scm_out_of_bounds);
  scm_c_define_gsubr("getch", 0, 0, 0, scm_getch);
  scm_c_primitive_load("scheme/curses.scm");
  SCM player_p = scm_variable_ref(scm_c_lookup("player"));
  SCM terrain_p = scm_variable_ref(scm_c_lookup("generate-terrain"));
  init_level();
  scm_call_0(terrain_p);
  while(!exit)
    {
      // grow_grass();
      clear();
      draw_level(player.z);
      render(player, '@' , WHITE, 2);
      refresh();
      player = scm_to_xyz(scm_call_2(player_p, scm_from_xyz(player), scm_from_int(getch())));
    }
  endwin();
  return(0);
}
