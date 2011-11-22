#include <cassert>
#include <iostream>

extern "C" {
#include <leptonica/allheaders.h>
}

#include "utils.h"


bool get_data(const char *f, std::vector<double> & output)
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

std::vector<double> read_data(const char *f1, const char *f2)
{
	std::vector<double> output;

	get_data(f1, output);
	get_data(f2, output);

	return output;
}


void print_result(std::vector<double> result, const char *f1, const char *f2)
{
	std::cout << f1 << " - " << f2;
	for (int i=0; i<result.size(); i++) {
		//int r = result[i] < 0.5 ? 0 : 1;
		double r = result[i];
		std::cout << "   " << r;
	}
	std::cout << std::endl;
}
