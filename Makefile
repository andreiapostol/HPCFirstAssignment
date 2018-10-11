stencil: stencil.c
	gcc -std=c99 -pg -Wall -O2 $^ -o $@

