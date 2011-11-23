#include <cassert>
#include <iostream>
#include <fstream>
#include <vector>

#include "nn.h"

NN::NN(int input_size, int output_size, int hidden_layers, int layer_size)
	: m_inputSize(input_size),
	  m_outputSize(output_size),
	  m_layers()
{
	m_layers.push_back(nnLayer(input_size, layer_size));
	for (int i=1; i<hidden_layers-1; i++) {
		m_layers.push_back(nnLayer(layer_size, layer_size));
	}
	m_layers.push_back(nnLayer(layer_size, output_size));
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


double NN::learn(std::vector<double> const& input, std::vector<double> const& result, double alpha, double gamma)
{
	assert(input.size() == m_inputSize);
	assert(result.size() == m_outputSize);

	double e=0.0;
	std::vector<double> err;
	std::vector<double> output = process(input);

	for (int i=0; i<output.size(); i++) {
		err.push_back(output[i] - result[i]);
		e += -1 * result[i]*log(output[i]) - (1.0-result[i])*log(1.0-output[i]);
	}

	for (int i=m_layers.size()-1; i>=0; i--) {
		err = m_layers[i].learn(err, alpha, gamma);
	}

	return e;
}


bool NN::save(const char *fname)
{
	std::fstream fs;
	fs.open(fname, std::fstream::out);

	fs << "input_size " << m_layers[0].inputSize() << "\n";
	fs << "output_size " << m_layers[m_layers.size()-1].size() << "\n";
	fs << "layers " << m_layers.size() << "\n";
	fs << m_layers[0].size();
	for (int i=1; i<m_layers.size(); i++) {
		fs << " " << m_layers[i].size();
	}
	fs << "\n";

	for (int i=0; i<m_layers.size(); i++) {
		std::vector<nnNode> const& nodes = m_layers[i].getNodes();
		for (int n=0; n<nodes.size(); n++) {
			std::vector<double> const& theta = nodes[n].getTheta();
			for (int t=0; t<theta.size(); t++) {
				fs << theta[t] << "\n";
			}
		}
	}

	fs.close();
	return true;
}


NN::NN(const char *fname)
	: m_inputSize(0),
	  m_outputSize(0),
	  m_layers()
{
	std::fstream fs;
	fs.open(fname, std::fstream::in);
	fs.width (25);

	char name[26];
	int layers;
	int layer_size;

	fs >> name;
#ifndef NOT_VERBOSE_DEBUG
	std::cout << "'" << name << "'" << std::endl;
#endif
	fs >> m_inputSize;

	fs >> name;
#ifndef NOT_VERBOSE_DEBUG
	std::cout << name << std::endl;
#endif
	fs >> m_outputSize;

	fs >> name;
#ifndef NOT_VERBOSE_DEBUG
	std::cout << name << std::endl;
#endif
	fs >> layers;

#ifndef NOT_VERBOSE_DEBUG
	std::cout << m_inputSize << " " << m_outputSize << " " << layers << "\n";
#endif

	int prev_size = m_inputSize;
	for (int i=0; i<layers; i++) {
		fs >> layer_size;
#ifndef NOT_VERBOSE_DEBUG
		std::cout << layer_size << std::endl;
#endif
		m_layers.push_back(nnLayer(prev_size, layer_size));
		prev_size = layer_size;
	}

#ifndef NOT_VERBOSE_DEBUG
	std::cout << "layers: " << m_layers.size() << std::endl;
#endif

	double theta;
	for (int i=0; i<m_layers.size(); i++) {
		std::vector<nnNode> nodes = m_layers[i].getNodes();
#ifndef NOT_VERBOSE_DEBUG
		std::cout << "nodes: " << nodes.size() << " " << nodes[0].inputSize() << std::endl;
#endif
		for (int n=0; n<nodes.size(); n++) {
			int tSize = nodes[n].getTheta().size();
			for (int t=0; t<tSize; t++) {
				fs >> theta;
#ifndef NOT_VERBOSE_DEBUG
				if (t==0)
					std::cout << theta << std::endl;
#endif
				m_layers[i].setTheta(n, t, theta);
			}
		}
	}

	fs.close();
}
