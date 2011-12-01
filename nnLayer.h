#ifndef __NN_LAYER_H
#define __NN_LAYER_H

#include <vector>

#include "nnNode.h"

class nnLayer {
	public:
		nnLayer(int input_size, int number_nodes);
		~nnLayer(void);

		double *process(double *input);
		double *learn(double *err, double alpha, double lambda);
		double update(int m, double alpha, double lambda);
	
		int size(void) const { return m_size; };
		int inputSize(void) const { return m_inputSize; };
		nnNode **getNodes(void) const { return p_nodes; };

		void setTheta(int n, int t, double value) { p_nodes[n]->setTheta(t, value); };

	private:
		int m_inputSize;
		int m_size;
		nnNode **p_nodes;
};

#endif
