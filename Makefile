stencil: stencil.c
	icc -std=c99 -O2 -g -march=native -ansi-alias \
	-qopt-report=5 -qopt-report-phase=vec -ipo -simd \
	-fp-speculation=fast -fma \
	-Wall $^ -o $@

