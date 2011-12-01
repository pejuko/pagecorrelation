#include <cassert>
#include <iostream>

extern "C" {
#include <leptonica/allheaders.h>
}

#include "utils.h"

#define SQUARED_MAX_INTENSITY 65025.0 // 255**2

bool get_data(const char *f, double *output, int size)
{
	PIX *pix = pixRead(f);
	assert(pix != NULL);

	PIX *pixg, *pix8, *pixs;
	pixg = pixRemoveColormap(pix, REMOVE_CMAP_TO_GRAYSCALE);
	pix8 = pixConvertTo8(pixg, FALSE);
	pixs = pixScaleToSize(pix8, size, size);

	assert(pixs->w == size);
	assert(pixs->h == size);

	pixDestroy(&pix);
	pixDestroy(&pixg);
	pixDestroy(&pix8);

	l_uint32 val;
	for (int y=0; y<pixs->h; y++) {
		for (int x=0; x<pixs->w; x++) {
			pixGetPixel(pixs, x, y, &val);
			output[y*pixs->w + x] = 1.0 - (double(val*val) / SQUARED_MAX_INTENSITY);
		}
	}

	pixDestroy(&pixs);

	return true;
}

double *read_data(const char *f1, const char *f2, int size)
{
	int data_size = size*size*2;
	double *output = (double*)malloc(sizeof(double)*data_size);
	for (int i=0; i<data_size; i++) {
		output[i] = 0.5;
	}

	get_data(f1, output, size);
	get_data(f2, output, size);

	return output;
}


void write_data(const char *f1, double *data, int size)
{
	int img_size = sqrt((size-1)/2);
	PIX *pix = pixCreate(img_size,img_size*2,8);
	for (int y=0; y<img_size*2; y++) {
		for (int x=0; x<img_size; x++) {
			int val = (int)(data[y*img_size+x+1] * 255.0);
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
