#include <cassert>
#include <vector>

#include "nn.h"


NN::NN(int input_size, int output_size, int hidden_layers, int layer_size, double alpha)
	: m_inputSize(input_size),
	  m_outputSize(output_size),
	  m_layers()
{
	m_layers.push_back(nnLayer(input_size, layer_size, alpha));
	for (int i=1; i<hidden_layers-1; i++) {
		m_layers.push_back(nnLayer(layer_size, layer_size, alpha));
	}
	m_layers.push_back(nnLayer(layer_size, output_size, alpha));
}


std::vector<double> NN::process(std::vector<double> const& input)
{
	assert(input.size() == m_inputSize);

	std::vector<double> output = input;
	for (int i=0; i<m_layers.size(); i++) {
		output = m_layers[i].process(output);
	}

	return output;
}


void NN::learn(std::vector<double> const& input, std::vector<double> const& result)
{
	assert(input.size() == m_inputSize);
	assert(result.size() == m_outputSize);

	std::vector<double> err;
	std::vector<double> output = process(input);

	for (int i=0; i<output.size(); i++) {
		err.push_back(result[i] - output[i]);
	}

	for (int i=m_layers.size()-1; i>=0; i--) {
		err = m_layers[i].learn(err);
	}
}
