#include "common.h"

void log_impl(std::ostream& os) {
	os << std::endl;
}

float random_float() {
	return ((double)rand() / (RAND_MAX));
}