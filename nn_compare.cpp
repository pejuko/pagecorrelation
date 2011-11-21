#include <cassert>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <iostream>

extern "C" {
#include <leptonica/allheaders.h>
}

#include "nn.h"


bool get_data(char *f, std::vector<double> & output)
{
	PIX *pix = pixRead(f);
	assert(pix != NULL);

	PIX *pixg, *pix8, *pixs;
	pixg = pixRemoveColormap(pix, REMOVE_CMAP_TO_GRAYSCALE);
	pix8 = pixConvertTo8(pixg, FALSE);
	pixs = pixScaleToSize(pix8, 200, 200);

	pixDestroy(&pix);
	pixDestroy(&pixg);
	pixDestroy(&pix8);

	l_uint32 val;
	for (int y=0; y<pixs->h; y++) {
		for (int x=0; x<pixs->w; x++) {
			pixGetPixel(pixs, x, y, &val);
			output.push_back(double(val));
		}
	}

	pixDestroy(&pixs);

	return true;
}

std::vector<double> read_data(char *f1, char *f2)
{
	std::vector<double> output;

	get_data(f1, output);
	get_data(f2, output);

	return output;
}


int main(int argc, char **argv)
{
	srandom( clock() + time(NULL) );
	NN nn = NN(80000, 2, 3, 20, 1.0);
	
	std::vector<double> data = read_data(argv[1], argv[2]);

	std::vector<double> result = nn.process(data);
	for (int i=0; i<result.size(); i++) {
		printf("%d: %f\n", i, result[i]);
	}

	nn.learn(data, result);
	//std::cout << err << std::endl;

	for (int i=0; i<10; i++) {
		result[0] = 1.0;
		result[1] = 0.0;
		nn.learn(data, result);
		//std::cout << "err: " << err << std::endl;
		result = nn.process(data);
		for (int i=0; i<result.size(); i++) {
			printf("%d: %f\n", i, result[i]);
		}
		std::cout << std::endl;
	}

	return 0;
}
