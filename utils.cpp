#include <cassert>
#include <iostream>

extern "C" {
#include <leptonica/allheaders.h>
}

#include "utils.h"


bool get_data(const char *f, double *output)
{
	PIX *pix = pixRead(f);
	assert(pix != NULL);

	PIX *pixg, *pix8, *pixs;
	pixg = pixRemoveColormap(pix, REMOVE_CMAP_TO_GRAYSCALE);
	pix8 = pixConvertTo8(pixg, FALSE);
	pixs = pixScaleToSize(pix8, 200, 200);

	assert(pixs->w == 200);
	assert(pixs->h == 200);

	pixDestroy(&pix);
	pixDestroy(&pixg);
	pixDestroy(&pix8);

	l_uint32 val;
	for (int y=0; y<pixs->h; y++) {
		for (int x=0; x<pixs->w; x++) {
			pixGetPixel(pixs, x, y, &val);
			output[y*pixs->w + x] = 1.0 - (double(val) / 255.0);
		}
	}

	pixDestroy(&pixs);

	return true;
}

double *read_data(const char *f1, const char *f2)
{
	double *output = (double*)malloc(sizeof(double)*200*200*2);
	for (int i=0; i<200*200*2; i++) {
		output[i] = 0.5;
	}

	get_data(f1, output);
	get_data(f2, output);

	return output;
}


void write_data(const char *f1, double *data, int size)
{
	PIX *pix = pixCreate(200,400,8);
	assert(size == 80001);
	for (int y=0; y<400; y++) {
		for (int x=0; x<200; x++) {
			int val = (int)(data[y*200+x+1] * 255.0);
			pixSetPixel(pix, x, y, val);
		};
	}
	pixWriteTiff(f1, pix, IFF_TIFF_LZW, "w");
	pixDestroy(&pix);
}


void print_result(double *result, int size, const char *f1, const char *f2)
{
	std::cout << f1 << " - " << f2;
	for (int i=0; i<size; i++) {
		//int r = result[i] < 0.5 ? 0 : 1;
		double r = result[i];
		std::cout << "   " << r;
	}
	std::cout << std::endl;
}
