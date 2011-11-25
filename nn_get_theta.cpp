#include <iostream>

#include "nn.h"

int main (int argc, char **argv)
{
	NN nn("data.nn");

	nnLayer **layers = nn.getLayers();
	for (int l=0; l<nn.size(); l++) {
		nnNode **nodes = layers[l]->getNodes();
		for (int n=0; n<layers[l]->size(); n++) {
			double *theta = nodes[n]->getTheta();
			for (int t=0; t<nodes[n]->inputSize()+1; t++) {
				std::cout << theta[t] << std::endl;
			}
		}
	}

	return 0;
}
