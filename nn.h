#ifndef __NN__H__
#define __NN_H__

#include <vector>
#include "nnLayer.h"

class NN {
	public:
		NN(int input_size, int output_size, int hidden_layers, int layer_size);
		NN(const char *fname);
		~NN(void);

		double *process(double *input);
		double learn(double *input, double *result, double alpha, double lambda);
		double update(int m, double alpha, double lambda);

		bool display(const char *fbase);
		bool save(const char *fname);
		bool load(const char *fname);

		int inputSize(void) const { return m_inputSize; };
		int outputSize(void) const { return m_outputSize; };
		int size(void) const { return m_size; };

		nnLayer **getLayers(void) { return p_layers; };
		double squaredTheta(void) const;

	private:
		int m_inputSize;
		int m_outputSize;
		int m_size;
		nnLayer **p_layers;
};

#endif
