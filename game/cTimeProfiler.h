#pragma once


//#define SHOW_TIMINGS 

#ifdef SHOW_TIMINGS

#include <chrono>
#include <ratio>

class cScopedTimeProfiler
{
	std::chrono::high_resolution_clock::time_point start;
	const char* text;
public:
	cScopedTimeProfiler(const char* text);
	~cScopedTimeProfiler();
};

#define ADD_SCOPED_TIME_PROFILER(s) cScopedTimeProfiler _p##__LINE__(s)

#else

#define ADD_SCOPED_TIME_PROFILER(s)

#endif