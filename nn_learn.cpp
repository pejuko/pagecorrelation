#include <cstdlib>
#include <fstream>
#include <iostream>
#include <ctime>
#include <string>

#include "utils.h"
#include "nn.h"


int main(int argc, char **argv)
{
	if (argc != 6) {
		std::cout << "Usage: nn_learn <alpha> <lambda> <image_dir> <image_size> <samples>" << std::endl;
		std::cout << "Example: cat set-train/list.txt | nn_learn 0.1 0.0 set-train/img/" << std::endl;
		std::cout << "It reads and update 'data.nn' file" << std::endl;
		return 1;
	}

	NN *nn;
	double alpha = atof(argv[1]);
	double lambda = atof(argv[2]);
	std::string img_dir(argv[3]);
	int img_size = atoi(argv[4]);

	srandom( clock() + time(NULL) );
	std::cout.precision(COUT_PRECISION);


	std::fstream fs;
	fs.open("data.nn");
	if (fs.is_open()) {
		fs.close();
		nn = new NN("data.nn");
	} else {
		nn = new NN(img_size*img_size*2, 1, 1, 30);
	}

	double *result = (double*)malloc(sizeof(double)*nn->outputSize());

	int samples = atoi(argv[5]);
	int iteration = 0;
	double e = 0.0;
	std::cin.width(1024);

	int sample=1;
	while (! std::cin.eof()) {
		int same=0, bad=0;
		std::string number;

		std::cin >> number;
		if (number.empty())
			break;

		if (number == "update") {
			double st = nn->squaredTheta();
			std::cout << "e: " << e << std::endl;
			std::cout << "st: " << st << std::endl;
			//e = e/samples + (lambda/(2*samples)) * st;
			e = e/samples;
			std::cout << "J = " << e << std::endl;
//			nn->update(samples, alpha, lambda);
			nn->save("data.nn");
			//samples = 0;
			sample = 1;
			e = 0.0;
			++iteration;
			continue;
		}
		std::cin >> same >> bad;

		std::string f1 = img_dir + "/" + number + "-1.tif";
		std::string f2 = img_dir + "/" + number + "-2.tif";


		result[0] = same;
		//result[1] = bad;
		double *data = read_data(f1.c_str(), f2.c_str(), img_size);
		double err = nn->learn(data, result, alpha, lambda);
		e += err;
		free(data);

		++sample;

		std::cout << "f1: " << f1 << "   f2: " << f2 << "   err: " << err << std::endl;
//		std::cout << "f1: " << f1 << "   f2: " << f2 << std::endl;
	}

	free(result);

//	if (sample == samples)
//		nn->update(samples, alpha, lambda);
	nn->save("data.nn");
	delete nn;

	return 0;
}
