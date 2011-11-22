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
	for (int i=0; i<=input_size; i++) {
		double r = float(random()) / RAND_MAX;
		m_theta.push_back(r - 0.5);
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

	for (int i=1; i<m_theta.size(); i++) {
		delta.push_back(m_theta[i] * err);
	}

	double reg = 0.0;
	for (int i=1; i<m_theta.size(); i++) {
		reg += m_theta[i];
	}
	reg = (gamma * reg) / m_theta.size();

	for (int i=1; i<m_theta.size(); i++) {
		m_theta[i] += alpha * delta[i] * (m_lastResult * (1-m_lastResult)) + reg;
	}

	return delta;
}
