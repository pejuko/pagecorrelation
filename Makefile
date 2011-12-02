PAGE_CORRELATION_SOURCES=page_correlation.c
PAGE_CORRELATION_HEADERS=
PAGE_CORRELATION_FLAGS=-Ofast

NN_SOURCES=nnNode.cpp nnLayer.cpp nn.cpp utils.cpp
NN_HEADERS=nnNode.h nnLayer.h nn.h utils.h
NN_LIBS=-llept -lstdc++ -lm
NN_COMPARE_SOURCES=${NN_SOURCES} nn_compare.cpp
NN_COMPARE_FLAGS=-DNOT_VERBOSE_DEBUG=1 -Ofast -funroll-loops -lpthread #-g -pg
NN_LEARN_SOURCES=${NN_SOURCES} nn_learn.cpp
NN_LEARN_FLAGS=-DNOT_VERBOSE_DEBUG=1 -O3 -funroll-loops -mmmx -msse -msse2 -g #-pg

all: page_correlation nn_compare nn_learn get_theta

page_correlation: ${PAGE_CORRELATION_SOURCES} ${PAGE_CORRELATION_HEADERS}
	gcc ${PAGE_CORRELATION_FLAGS} -o page_correlation ${PAGE_CORRELATION_SOURCES} -llept

nn_compare: ${NN_COMPARE_SOURCES} ${NN_HEADERS}
	gcc ${NN_COMPARE_FLAGS} -o nn_compare ${NN_COMPARE_SOURCES} ${NN_LIBS}

nn_learn: ${NN_LEARN_SOURCES} ${NN_HEADERS}
	gcc ${NN_LEARN_FLAGS} -o nn_learn ${NN_LEARN_SOURCES} ${NN_LIBS}

get_theta: ${NN_HEADERS} ${NN_SOURCES} nn_get_theta.cpp
	gcc ${NN_COMPARE_FLAGS} -o nn_get_theta ${NN_SOURCES} nn_get_theta.cpp -g ${NN_LIBS}

debug:
	gcc -g -o page_correlation page_correlation.c -llept -DDEBUG

test: page_correlation
	./page_correlation ../samples/PICT0004.JPG ../samples/PICT0005.JPG

clean:
	rm -f page_correlation nn_compare nn_learn
