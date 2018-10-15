stencil: stencil.c
	gcc -ftree-vectorize -ftree-vectorizer-verbose=2 -S -ffast-math -std=c99 -Wall -O2 $^ -o $@
