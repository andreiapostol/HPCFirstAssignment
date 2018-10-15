import sys
file = open("results.txt", "r")
sum = 0

for i in file.read().split():
    sum += float(i)

print(sum)


# stencilPadding cu O2: 23.31241
# stencilPadding cu O3: 24.628428


# python check.py --ref-stencil-file stencil_1024_1024_100.pgm --stencil-file stencil.pgm
# rm results.txt; for i in `seq 1 100`; do ./stencilPadding 1024 1024 100 | head -n 2 | tail -n 1| awk '{ print $2}' >> results.txt; done
