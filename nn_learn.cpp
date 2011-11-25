#include <cstdlib>
#include <fstream>
#include <iostream>
#include <ctime>
#include <string>

#include "utils.h"
#include "nn.h"


int main(int argc, char **argv)
{
	if (argc != 4) {
		std::cout << "Usage: nn_learn <alpha> <lambda> <image_dir>" << std::endl;
		std::cout << "Example: cat set-train/list.txt | nn_learn 0.1 0.0 set-train/img/" << std::endl;
		std::cout << "It reads and update 'data.nn' file" << std::endl;
		return 1;
	}

	NN *nn;
	double alpha = atof(argv[1]);
	double lambda = atof(argv[2]);

	srandom( clock() + time(NULL) );

	std::fstream fs;
	fs.open("data.nn");
	if (fs.is_open()) {
		fs.close();
		nn = new NN("data.nn");
	} else {
		//nn = new NN(80000, 2, 5, 40);
		nn = new NN(80000, 1, 3, 30);
	}

	std::string img_dir(argv[3]);

	double *result = (double*)malloc(sizeof(double)*nn->outputSize());

	int samples = 0;
	std::cin.width(1024);
	while (! std::cin.eof()) {
		int same=0, bad=0;
		std::string number;

		std::cin >> number >> same >> bad;
		if (number.empty())
			break;

		std::string f1 = img_dir + "/" + number + "-1.tif";
		std::string f2 = img_dir + "/" + number + "-2.tif";


		result[0] = same;
		//result[1] = bad;
		double *data = read_data(f1.c_str(), f2.c_str());
		double err = nn->learn(data, result, alpha, lambda);
		free(data);

		++samples;
		if ((samples%1000) == 0) {
			nn->save("data.nn");
		}

		std::cout << "f1: " << f1 << "   f2: " << f2 << "   err: " << err << std::endl;
//		std::cout << "f1: " << f1 << "   f2: " << f2 << std::endl;
	}

	free(result);

	nn->save("data.nn");
	delete nn;

	return 0;
}
