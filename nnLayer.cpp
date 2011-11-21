#include <cstdio>
#include <cassert>
#include "nnLayer.h"


nnLayer::nnLayer(int input_size, int number_nodes, double alpha)
	: m_inputSize(input_size),
	  m_nodes()
{
	for (int i=0; i<number_nodes; i++) {
		m_nodes.push_back(nnNode(input_size, alpha));
	}
}


std::vector<double> nnLayer::process(std::vector<double> const& input)
{
	assert(input.size() == m_inputSize);

	std::vector<double> output;
	for (int i=0; i<m_nodes.size(); i++) {
		output.push_back(m_nodes[i].process(input));
	}

	return output;
}


std::vector<double> nnLayer::learn(std::vector<double> err)
{
	std::vector<std::vector<double> > e;
	for (int i=0; i<m_nodes.size(); i++) {
		e.push_back(m_nodes[i].learn(err[i]));
	}

	int rows = e.size();
	int cols = e[0].size();
	std::vector<double> delta;
	for (int c=0; c<cols; c++) {
		double d = 0.0;
		for (int r=0; r<rows; r++) {
			d += e[r][c];
		}
		delta.push_back(d);
	}

	//printf ("deltas: %d\n", delta.size());
	if (delta.size() < 20) {
		for (int i=0; i<delta.size(); i++) {
			printf(" %f ", delta[i]);
		}
		printf ("\n");
	}

	return delta;
}