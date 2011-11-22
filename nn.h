#ifndef __NN__H__
#define __NN_H__

#include <vector>
#include "nnLayer.h"

class NN {
	public:
		NN(int input_size, int output_size, int hidden_layers, int layer_size);
		NN(const char *fname);

		std::vector<double> process(std::vector<double> const& input);
		void learn(std::vector<double> const& input, std::vector<double> const& result, double alpha=0.1, double gamma=1.0);

		bool save(const char *fname);
		bool load(const char *fname);

	private:
		int m_inputSize;
		int m_outputSize;
		std::vector<nnLayer> m_layers;
};

#endif
