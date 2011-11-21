#ifndef __NN_NODE_H_
#define __NN_NODE_H_

#include <cmath>
#include <vector>

class nnNode {
	public:
		nnNode(int input_size, double alpha=0.1);
		double process(std::vector<double> const& input);
		std::vector<double> learn(double err);

	private:
		double g(double f) const { return 1.0/(1+exp(-1*f)); };

		int m_inputSize;
		double m_alpha;
		double m_error;
		double m_lastResult;
		std::vector<double> m_theta;
};

#endif
