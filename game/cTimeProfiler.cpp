#include "cTimeProfiler.h"
#ifdef SHOW_TIMINGS
#include "cTools.h"
cScopedTimeProfiler::cScopedTimeProfiler(const char* text)
{
	this->text = text;
	startTime = std::chrono::high_resolution_clock::now();
}

cScopedTimeProfiler::~cScopedTimeProfiler()
{
	std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> diff = end - startTime;
	printf("%s : %lf\n", text, diff.count());
}


cTimeProfiler::cTimeProfiler(const char* text)
{
	strncpy_s(this->text, text, sizeof(this->text));
	started = false;
}

cTimeProfiler::~cTimeProfiler()
{
	stop();
}

void cTimeProfiler::start()
{
	startTime = std::chrono::high_resolution_clock::now();
	started = true;
}

void cTimeProfiler::stop()
{
	if (started)
	{
		started = false;

		std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> diff = end - startTime;
		printf("%s : %lf\n", text, diff.count());
	}
}

cAccumulatedTimeProfiler::cAccumulatedTimeProfiler(const char* text)
{
	strncpy_s(this->text, text, sizeof(this->text));
	started = false;
	reset();
}

cAccumulatedTimeProfiler::~cAccumulatedTimeProfiler()
{
}

void cAccumulatedTimeProfiler::start()
{
	assert(started == false);
	startTime = std::chrono::high_resolution_clock::now();
	started = true;
}

void cAccumulatedTimeProfiler::stop()
{
	assert(started);
	if (started)
	{
		started = false;

		std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> diff = end - startTime;
		total += diff.count();
	}
}

void cAccumulatedTimeProfiler::printResult()
{
	assert(started == false);
	if (total > 0.0)
	{
		printf("%s : %lf\n", text, total);
	}
}

void cAccumulatedTimeProfiler::reset()
{
	assert(started == false);
	total = 0.0;
}


#endif