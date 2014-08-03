all:
	gcc -Wall -Werror -O2 `pkg-config --cflags --libs ncurses guile-2.0` src/curses.c -o curses
