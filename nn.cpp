#include <cassert>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "utils.h"
#include "nn.h"

NN::NN(int input_size, int output_size, int hidden_layers, int layer_size)
	: m_inputSize(input_size),
	  m_outputSize(output_size),
	  m_size(hidden_layers+1)
{
	p_layers = (nnLayer**)malloc(sizeof(nnLayer*)*m_size);
	p_layers[0] = new nnLayer(input_size, layer_size);
	for (int i=1; i<hidden_layers; i++) {
		p_layers[i] = new nnLayer(layer_size, layer_size);
	}
	p_layers[m_size-1] = new nnLayer(layer_size, output_size);
}


NN::~NN(void)
{
	for (int i=0; i<m_size; i++) {
		delete p_layers[i];
		p_layers[i] = 0;
	}
	free(p_layers);
	p_layers = 0;
}


double *NN::process(double *input)
{
	double *output = input;
	double *tmp;

	/*
	for (int i=0; i<m_inputSize; i++) {
		if (isnan(input[i]))
			std::cout << "nan" << std::endl;
	}
	*/

	for (int i=0; i<m_size; i++) {
		tmp = p_layers[i]->process(output);
		if (output != input) free(output);
		output = tmp;
	}

	return output;
}


double NN::learn(double *input, double *result, double alpha, double lambda)
{
	double e=0.0;
	double *err = (double*)malloc(sizeof(double)*m_outputSize);
	double *output = process(input);

	for (int i=0; i<m_outputSize; i++) {
		err[i] = output[i] - result[i];
		//e = NORM_DOUBLE(e, e + -1 * result[i]*log(output[i]) - (1.0-result[i])*log(1.0-output[i]));
		e = NORM_DOUBLE( e, (result[i]==1) ? (e - log(output[i])) : (e - log(1.0-output[i])) );
	}

	double *tmp;
	for (int i=m_size-1; i>=0; i--) {
		tmp = p_layers[i]->learn(err, alpha, lambda);
		free(err);
		err = tmp;
	}
	free(err);

	return e;
}


double NN::update(int m, double alpha, double lambda)
{
	for (int l=0; l<m_size; l++) {
		p_layers[l]->update(m, alpha, lambda);
	}
}


double NN::squaredTheta(void) const
{
	nnNode **nodes;
	double *theta;
	double st = 0.0;
	for (int i=0; i<m_size; i++) {
		nodes = p_layers[i]->getNodes();
		for (int n=0; n<p_layers[i]->size(); n++) {
			theta = nodes[n]->getTheta();
			for (int t=1; t<(nodes[n]->inputSize()+1); t++) {
				st += theta[t] * theta[t];
				/*
				if (isinf(st)) {
					std::cout << "theta[t]: " << theta[t] << std::endl;
				}
				assert(! isinf(st));
				*/
			}
		}
	}
	return st;
}


bool NN::display(const char *fbase)
{
	nnNode *node;
	for (int i=0; i<p_layers[0]->size(); i++) {
		std::stringstream fname;
		fname << fbase << i << ".tif";
		node = p_layers[0]->getNodes()[i];
		write_data(fname.str().c_str(), node->getTheta(), node->inputSize()+1);
	}
}


bool NN::save(const char *fname)
{
	std::ofstream fs(fname, std::fstream::out | std::fstream::binary);

	fs << "double_size " << sizeof(double) << std::endl;
	fs << "input_size " << p_layers[0]->inputSize() << "\n";
	fs << "output_size " << p_layers[m_size-1]->size() << "\n";
	fs << "layers " << m_size << "\n";
	fs << p_layers[0]->size();
	for (int i=1; i<m_size; i++) {
		fs << " " << p_layers[i]->size();
	}
	fs << "\n";


	nnNode **nodes;
	double *theta;
	for (int i=0; i<m_size; i++) {
		nodes = p_layers[i]->getNodes();
#ifndef NOT_VERBOSE_DEBUG
		std::cout << nodes[0]->getTheta(0) << std::endl;
#endif
		for (int n=0; n<p_layers[i]->size(); n++) {
			theta = nodes[n]->getTheta();
			for (int t=0; t<(nodes[n]->inputSize()+1); t++) {
				//fs << theta[t] << "\n";
				fs.write((const char*)(&theta[t]), sizeof(double));
			}
		}
	}

	fs.close();
	return true;
}


NN::NN(const char *fname)
	: m_inputSize(0),
	  m_outputSize(0),
	  m_size(0)
{
	std::ifstream fs(fname, std::ifstream::in | std::ifstream::binary );
	fs.width (25);

	char name[26];
	int layers;
	int layer_size;
	int double_size;

	fs >> name;
	fs >> double_size;
	assert(double_size == sizeof(double));

	fs >> name;
	fs >> m_inputSize;

	fs >> name;
	fs >> m_outputSize;

	fs >> name;
	fs >> m_size;

	p_layers = (nnLayer**)malloc(sizeof(nnLayer*)*m_size);

	int prev_size = m_inputSize;
	for (int i=0; i<m_size; i++) {
		fs >> layer_size;
		p_layers[i] = new nnLayer(prev_size, layer_size);
		prev_size = layer_size;
	}

	double theta;
	nnNode **nodes;
	fs.seekg(1, std::ios_base::cur); // '\n'
	for (int i=0; i<m_size; i++) {
		nodes = p_layers[i]->getNodes();
#ifndef NOT_VERBOSE_DEBUG
		std::cout << "nodes: " << nodes.size() << " " << nodes[0].inputSize() << std::endl;
#endif
		for (int n=0; n<p_layers[i]->size(); n++) {
			int tSize = nodes[n]->inputSize()+1;
			for (int t=0; t<tSize; t++) {
				//fs >> theta;
				fs.read((char*)(&theta), double_size);
				p_layers[i]->setTheta(n, t, theta);
			}
		}
#ifndef NOT_VERBOSE_DEBUG
		std::cout << nodes[0]->getTheta(0) << std::endl;
#endif
	}

	fs.close();
}
