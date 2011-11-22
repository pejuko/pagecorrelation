#ifndef __NN_LAYER_H
#define __NN_LAYER_H

#include <vector>

#include "nnNode.h"

class nnLayer {
	public:
		nnLayer(int input_size, int number_nodes);
		std::vector<double> process(std::vector<double> const& input);
		std::vector<double> learn(std::vector<double> err, double alpha=0.1, double gamma=1.0);
	
	private:
		int m_inputSize;
		std::vector<nnNode> m_nodes;
};

#endif
