//#define NDEBUG

#include <cassert>
#include <cstdlib>
#include <vector>

#include "nnNode.h"


nnNode::nnNode(int input_size)
	: m_inputSize(input_size),
	  m_error(0.0),
	  m_lastResult(0.0)
{
	p_theta = (double*)malloc(sizeof(double)*(m_inputSize+1));

	double r;
	double epsilon = 0.12;
	for (int i=0; i<=input_size; i++) {
		r = (float(random()) / RAND_MAX);
		p_theta[i] = r * 2 * epsilon - epsilon;
	}
}

nnNode::~nnNode(void)
{
	free(p_theta);
	p_theta = 0;
}


double nnNode::process(double *input)
{
	double result = p_theta[0];
	for (int i=0; i<m_inputSize; i++) {
		result += p_theta[i+1] * input[i];
	}

	//m_lastResult = g(result);
	m_lastResult = 1.0 / (1 + exp(-1*result));
	return m_lastResult;
}


double *nnNode::learn(double err, double alpha, double lambda)
{
	int i;

	double e = err/double(m_inputSize);
	double *delta = (double*)malloc(sizeof(double)*(m_inputSize+1));
	double g_deriv = m_lastResult * (1.0-m_lastResult);

	for (i=0; i<(m_inputSize+1); i++) {
		delta[i] = p_theta[i] * e * g_deriv;
	}

	double alpha_d = alpha * err * m_lastResult;
	p_theta[0] -= alpha_d;

	for (i=1; i<(m_inputSize+1); i++) {
		p_theta[i] -= alpha_d + lambda*p_theta[i];
	}

	return delta;
}
