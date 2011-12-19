#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <thread>

#include "nnLayer.h"
#include "utils.h"


nnLayer::nnLayer(int input_size, int number_nodes)
	: m_inputSize(input_size),
	  m_size(number_nodes)
{
	p_nodes = (nnNode**)malloc(sizeof(nnNode*)*m_size);
	for (int i=0; i<m_size; i++) {
		p_nodes[i] = new nnNode(input_size);
	}
}


double nnLayer::update(int m, double alpha, double lambda)
{
	for (int n=0; n<m_size; n++) {
		p_nodes[n]->update(m, alpha, lambda);
	}
}


nnLayer::~nnLayer(void)
{
	for (int i=0; i<m_size; i++) {
		delete p_nodes[i];
		p_nodes[i] = 0;
	}
	free(p_nodes);
	p_nodes = 0;
}



void processCPU(nnNode **nodes, double *input, double *output, int start, int end)
{
	for (int i=start; i<end; i++) {
		output[i] = nodes[i]->process(input);
	}
}


double *nnLayer::process(double *input)
{
	double *output = (double*)malloc(sizeof(double)*m_size);

	/*
	for (int i=0; i<m_size; i++) {
		output[i] = p_nodes[i]->process(input);
	}
	*/
	
	std::thread threads[NUM_CPU];

	int start = 0;
	int step = m_size / NUM_CPU;
	int end = start + step;
	for (int cpu=0; cpu<NUM_CPU; ++cpu, start+=step, end+=step) {
		if (cpu == NUM_CPU-1)
			end = m_size;
		threads[cpu] = std::thread(processCPU, p_nodes, input, output, start, end);
	}

	for (int cpu=0; cpu<NUM_CPU; ++cpu) {
		threads[cpu].join();
	}

	return output;
}


void learnCPU(nnNode **nodes, double *err, double **deltas, double alpha, double lambda, int start, int end )
{
	for (int i=start; i<end; ++i) {
		deltas[i] = nodes[i]->learn(err[i], alpha, lambda);
	}
}

double *nnLayer::learn(double *err, double alpha, double lambda)
{
	//double **e = (double**)malloc(sizeof(double*)*m_size);

	double *delta = (double*)malloc(sizeof(double)*(m_inputSize));
	for (int i=0; i<m_inputSize; ++i) {
		delta[i] = 0.0;
	}

	double *tmp;
	int c;
	for (int i=0; i<m_size; ++i) {
		tmp = p_nodes[i]->learn(err[i], alpha, lambda);
		for (c=0; c<m_inputSize; ++c) delta[c] += tmp[c];
		free(tmp);
	}

	/*
	// parallel computation
	//double **deltas = new double*[m_size];
	double **deltas = (double**)malloc(sizeof(double*)*m_size);

	std::thread threads[NUM_CPU];

	int start = 0;
	int step = m_size / NUM_CPU;
	int end = start + step;
	for (int cpu=0; cpu<NUM_CPU; ++cpu, start+=step, end+=step) {
		if (cpu == NUM_CPU-1)
			end = m_size;
		threads[cpu] = std::thread(learnCPU, p_nodes, err, deltas, alpha, lambda, start, end);
	}

	for (int cpu=0; cpu<NUM_CPU; ++cpu) {
		threads[cpu].join();
	}

	for (int r=0; r<m_size; ++r) {
		for (int c=0; c<m_inputSize; ++c) {
			delta[c] += deltas[r][c];
		}
		free(deltas[r]);
//		deltas[r] = 0;
	}

	free(deltas);
	//delete []deltas;
	// end parallel
	*/

	/*
	int cols = m_inputSize;
	double *delta = (double*)malloc(sizeof(double)*(cols-1));
	double d;
	int c, r;
	for (c=1; c<cols; c++) {
		d = 0.0;
		for (r=0; r<m_size; r++) {
			d = NORM_DOUBLE(d, d + e[r][c]);
		}
		delta[c-1] = d;
	}
	for (int r=0; r<m_size; r++) {
		free(e[r]);
	}
	free(e);
	*/

	return delta;
}
