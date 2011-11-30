#ifndef __UTILS_H__
#define __UTILS_H__

#ifndef COUT_PRECISION
#define COUT_PRECISION 15
#endif

#include <math.h>

//#define NORM_DOUBLE(x) (isnormal(x) ? (x) : (isnan(x) ? 0.0 : (isinf(x) ? 1.0 : x)))
#define NORM_DOUBLE(x, y) ((isnan(y) || isinf(y)) ? (x) : (y))

extern bool get_data(const char *f, double *output);
extern double *read_data(const char *f1, const char *f2);
extern void write_data(const char *f1, double *data, int size);
extern void print_result(double *result, int size, const char *f1, const char *f2);

#endif
