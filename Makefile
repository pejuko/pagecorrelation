PAGE_CORRELATION_SOURCES=page_correlation.c
PAGE_CORRELATION_HEADERS=

NN_COMPARE_SOURCES=nnNode.cpp nnLayer.cpp nn.cpp nn_compare.cpp
NN_COMPARE_HEADERS=nnNode.h nnLayer.h nn.h

all: page_correlation nn_compare

page_correlation: ${PAGE_CORRELATION_SOURCES} ${PAGE_CORRELATION_HEADERS}
	gcc -O3 -o page_correlation ${PAGE_CORRELATION_SOURCES} -llept

nn_compare: ${NN_COMPARE_SOURCES} ${NN_COMPARE_HEADERS}
	gcc -g -o nn_compare ${NN_COMPARE_SOURCES} -lstdc++ -llept

debug:
	gcc -g -o page_correlation page_correlation.c -llept -DDEBUG

test: page_correlation
	./page_correlation ../samples/PICT0004.JPG ../samples/PICT0005.JPG

clean:
	rm -f page_correlation nn_compare
