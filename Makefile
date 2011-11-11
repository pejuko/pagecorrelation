all: page_correlation

page_correlation: page_correlation.c
	gcc -O3 -o page_correlation page_correlation.c -llept

debug:
	gcc -g -o page_correlation page_correlation.c -llept -DDEBUG

test: page_correlation
	./page_correlation ../samples/PICT0004.JPG ../samples/PICT0005.JPG
