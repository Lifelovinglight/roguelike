tree-test:
	gcc -Wall -Werror --std=c89 -O2 tree.c tree_test.c -o tree_test
	valgrind --leak-check=full --track-origins=yes ./tree_test
