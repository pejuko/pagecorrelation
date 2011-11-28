#ifndef __NN_NODE_H_
#define __NN_NODE_H_

#include "utils.h"

class nnNode {
	public:
		nnNode(int input_size);
		~nnNode(void);

		double process(double *input);
		double *learn(double err);
		double update(int m, double alpha=0.1, double lambda=1.0);

		int inputSize() const { return m_inputSize; };
		double *getTheta(void) { return p_theta; };
		double getTheta(int i) const { return p_theta[i]; };

		void setTheta(int i, double value) { p_theta[i] = value; };

	private:
		double g(double f) const { return NORM_DOUBLE(1.0/(1+exp(-1*f))); };

		int m_inputSize;
		double m_lastResult;
		double *p_theta;
		double *p_grad;
};

#endif
