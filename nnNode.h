#ifndef __NN_NODE_H_
#define __NN_NODE_H_

#include <cmath>

class nnNode {
	public:
		nnNode(int input_size);
		~nnNode(void);

		double process(double *input);
		double *learn(double err, double alpha=0.1, double lambda=1.0);

		int inputSize() const { return m_inputSize; };
		double *getTheta(void) { return p_theta; };
		double getTheta(int i) const { return p_theta[i]; };

		void setTheta(int i, double value) { p_theta[i] = value; };

	private:
		double g(double f) const { return 1.0/(1+exp(-1*f)); };

		int m_inputSize;
		double m_alpha;
		double m_error;
		double m_lastResult;
		double *p_theta;
};

#endif
