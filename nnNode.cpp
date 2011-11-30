//#define NDEBUG

#include <cassert>
#include <cstdlib>
#include <iostream>

#include <math.h>

#include "nnNode.h"
#include "utils.h"


nnNode::nnNode(int input_size)
	: m_inputSize(input_size),
	  m_lastResult(0.0)
{
	p_theta = (double*)malloc(sizeof(double)*(m_inputSize+1));
	p_grad = (double*)malloc(sizeof(double)*(m_inputSize+1));

	for (int i=0; i<=input_size; i++) {
		p_theta[i] = genTheta();
		p_grad[i] = 0.0;
	}
}

nnNode::~nnNode(void)
{
	free(p_theta);
	p_theta = 0;
	free(p_grad);
	p_grad = 0;
}


double nnNode::process(double *input)
{
	double result = p_theta[0];
	for (int i=0; i<m_inputSize; i++) {
		result = NORM_DOUBLE(result, result + p_theta[i+1] * input[i]);
	}

	//m_lastResult = g(result);
	m_lastResult = 1.0 / (1 + exp(-1*result));
	return m_lastResult;
}


double *nnNode::learn(double err)
{
	int i;

	double *delta = (double*)malloc(sizeof(double)*(m_inputSize+1));
	double g_deriv = NORM_DOUBLE(g_deriv, m_lastResult * (1.0-m_lastResult));
	double grad = m_lastResult * err;

	for (i=0; i<(m_inputSize+1); i++) {
		delta[i] = NORM_DOUBLE(delta[i], p_theta[i] * err * g_deriv);
		p_grad[i] = NORM_DOUBLE(p_grad[i], p_grad[i] + grad);
	}

//	double alpha_d = NORM_DOUBLE(alpha * err * m_lastResult);
//	p_theta[0] -= alpha_d;

//	for (i=1; i<(m_inputSize+1); i++) {
//		p_theta[i] = NORM_DOUBLE(p_theta[i] - alpha_d + lambda*p_theta[i]);
//	}

	return delta;
}


double nnNode::update(int m, double alpha, double lambda)
{
	p_theta[0] = NORM_DOUBLE(p_theta[0], p_theta[0] - alpha * (p_grad[0]/m));
	p_grad[0] = 0.0;
	for (int i=1; i<m_inputSize+1; i++) {
		p_theta[i] = NORM_DOUBLE(p_theta[i], p_theta[i] - alpha * ((p_grad[i] + lambda * p_theta[i]) / m));
//		if (abs(p_theta[i]) < 1.0e-100)
//			p_theta[i] = 1.0e-100;
//			p_theta[i] = genTheta();
		p_grad[i] = 0.0;
	}
}
