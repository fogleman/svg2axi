all: svg2axi

svg2axi: svg2axi.c
	gcc -std=c99 -O3 -o svg2axi svg2axi.c
