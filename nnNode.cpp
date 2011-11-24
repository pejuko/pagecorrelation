//#define NDEBUG

#include <cassert>
#include <cstdlib>
#include <vector>

#include "nnNode.h"


nnNode::nnNode(int input_size)
	: m_inputSize(input_size),
	  m_error(0.0),
	  m_lastResult(0.0),
	  m_theta()
{
	double epsilon = 0.12;

	for (int i=0; i<=input_size; i++) {
		double r = (float(random()) / RAND_MAX);
		m_theta.push_back(r * 2 * epsilon - epsilon);
	}
}


double nnNode::process(std::vector<double> const& input)
{
	assert(input.size() == m_inputSize);

	double result = m_theta[0];
	for (int i=0; i<m_inputSize; i++) {
		result += m_theta[i+1] * input[i];
	}

	m_lastResult = g(result);
	return m_lastResult;
}


std::vector<double> nnNode::learn(double err, double alpha, double gamma)
{
	std::vector<double> delta;

//	double e = err/m_theta.size();
	double e = err;

	for (int i=0; i<m_theta.size(); i++) {
		delta.push_back(m_theta[i] * e * (m_lastResult * (1.0-m_lastResult)));
	}

	m_theta[0] -= alpha * err * m_lastResult;
	for (int i=1; i<m_theta.size(); i++) {
		m_theta[i] -= alpha * err * m_lastResult + gamma*m_theta[i]/2.0;
	}

	return delta;
}
