#ifndef __NN_NODE_H_
#define __NN_NODE_H_

#include <cmath>
#include <vector>

class nnNode {
	public:
		nnNode(int input_size);
		double process(std::vector<double> const& input);
		std::vector<double> learn(double err, double alpha=0.1, double gamma=1.0);

		int inputSize() const { return m_inputSize; };
		std::vector<double> const& getTheta(void) const { return m_theta; };
		double getTheta(int i) const { return m_theta[i]; };

		void setTheta(int i, double value) { m_theta[i] = value; };

	private:
		double g(double f) const { return 1.0/(1+exp(-1*f)); };

		int m_inputSize;
		double m_alpha;
		double m_error;
		double m_lastResult;
		std::vector<double> m_theta;
};

#endif
