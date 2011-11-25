#include <cassert>
#include <cstdlib>
#include <string.h>
#include <vector>
#include <iostream>

#include "nn.h"
#include "utils.h"


int compare(NN *nn, const char *f1, const char *f2)
{
	double *data = read_data(f1, f2);
	double *result = nn->process(data);

	print_result(result, nn->outputSize(), f1, f2);

	free(data);
	free(result);
}

int main(int argc, char **argv)
{
	if ((argc <= 1) || (argc==2 && argv[1][0] != '-') || (argv[1][0]!='-' && argc != 3) || (argc==1)){
		std::cout << "Usage: nn_compare <img1|-> [img2]" << std::endl;
		std::cout << "Example: nn_compare file1.jpg file2.jpg" << std::endl;
		std::cout << "Example: cat list_of_pairs.txt | nn_compare -" << std::endl;
		return 1;
	}
	
	NN nn("data.nn");

	if (argv[1][0] == '-') {
		while (! std::cin.eof()) {
			std::string f1, f2;
			std::cin >> f1 >> f2;
			if (f1.empty() || f2.empty())
				continue;
			compare(&nn, f1.c_str(), f2.c_str());
		}
	} else {
		nn.display("display_");
		if (compare(&nn, argv[1], argv[2]) >= 0.5)
			return 0;
		else
			return 1;
	}

	return 0;
}
