PAGE_CORRELATION_SOURCES=page_correlation.c
PAGE_CORRELATION_HEADERS=
PAGE_CORRELATION_FLAGS=-Ofast

NN_SOURCES=nnNode.cpp nnLayer.cpp nn.cpp utils.cpp
NN_HEADERS=nnNode.h nnLayer.h nn.h utils.h
NN_COMPARE_SOURCES=${NN_SOURCES} nn_compare.cpp
NN_COMPARE_FLAGS=-Ofast -DNOT_VERBOSE_DEBUG=1 #-g
NN_LEARN_SOURCES=${NN_SOURCES} nn_learn.cpp
NN_LEARN_FLAGS=-Ofast -DNOT_VERBOSE_DEBUG=1

all: page_correlation nn_compare nn_learn

page_correlation: ${PAGE_CORRELATION_SOURCES} ${PAGE_CORRELATION_HEADERS}
	gcc ${PAGE_CORRELATION_FLAGS} -o page_correlation ${PAGE_CORRELATION_SOURCES} -llept

nn_compare: ${NN_COMPARE_SOURCES} ${NN_HEADERS}
	gcc ${NN_COMPARE_FLAGS} -o nn_compare ${NN_COMPARE_SOURCES} -lstdc++ -llept

nn_learn: ${NN_LEARN_SOURCES} ${NN_HEADERS}
	gcc ${NN_LEARN_FLAGS} -o nn_learn ${NN_LEARN_SOURCES} -lstdc++ -llept

debug:
	gcc -g -o page_correlation page_correlation.c -llept -DDEBUG

test: page_correlation
	./page_correlation ../samples/PICT0004.JPG ../samples/PICT0005.JPG

clean:
	rm -f page_correlation nn_compare nn_learn
