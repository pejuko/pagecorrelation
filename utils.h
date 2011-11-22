#include <vector>

extern bool get_data(const char *f, std::vector<double> & output);
extern std::vector<double> read_data(const char *f1, const char *f2);
extern void print_result(std::vector<double> result, const char *f1, const char *f2);
