extern bool get_data(const char *f, double *output);
extern double *read_data(const char *f1, const char *f2);
extern void write_data(const char *f1, double *data, int size);
extern void print_result(double *result, int size, const char *f1, const char *f2);
