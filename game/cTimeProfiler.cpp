#include "cTimeProfiler.h"

#ifdef SHOW_TIMINGS

cScopedTimeProfiler::cScopedTimeProfiler(const char* text)
{
	this->text = text;

	start = std::chrono::high_resolution_clock::now();
}

cScopedTimeProfiler::~cScopedTimeProfiler()
{
#ifdef SHOW_TIMINGS
	std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> diff = end - start;
	printf("%s : %f\n", text, diff.count());
#endif
}

#endif