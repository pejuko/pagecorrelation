#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <iostream>

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


double *nnLayer::process(double *input)
{
	double *output = (double*)malloc(sizeof(double)*m_size);

	for (int i=0; i<m_size; i++) {
		output[i] = p_nodes[i]->process(input);
	}

	return output;
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
	for (int i=0; i<m_size; i++) {
		tmp = p_nodes[i]->learn(err[i], alpha, lambda);
		for (c=0; c<m_inputSize; ++c) delta[c] += tmp[c+1];
		free(tmp);
	}

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
