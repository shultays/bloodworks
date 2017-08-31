#pragma once

//#define SHOW_TIMINGS 

#ifdef SHOW_TIMINGS

#include <chrono>
#include <ratio>

class cScopedTimeProfiler
{
	std::chrono::high_resolution_clock::time_point startTime;
	const char* text;
public:
	cScopedTimeProfiler(const char* text);
	~cScopedTimeProfiler();
};

#define ADD_SCOPED_TIME_PROFILER(s) cScopedTimeProfiler _p##__LINE__(s)

class cTimeProfiler
{
	std::chrono::high_resolution_clock::time_point startTime;
	char text[64];
	bool started;
public:
	~cTimeProfiler();
	cTimeProfiler(const char* text);
	void start();
	void stop();
};


class cAccumulatedTimeProfiler
{
	std::chrono::high_resolution_clock::time_point startTime;
	char text[64];
	bool started;
	double total;
public:
	cAccumulatedTimeProfiler(){}
	cAccumulatedTimeProfiler(const char* text);
	~cAccumulatedTimeProfiler();
	void start();
	void stop();
	void printResult();
	void reset();
};

#else

#define ADD_SCOPED_TIME_PROFILER(s)

class cTimeProfiler
{
public:
	cTimeProfiler(const char* text){}
	void start() {};
	void stop() {};
};

class cAccumulatedTimeProfiler
{
public:
	cAccumulatedTimeProfiler() {}
	cAccumulatedTimeProfiler(const char* text) {}
	void start() {}
	void stop() {}
	void printResult() {}
	void reset() {}
};

class cScopedTimeProfiler
{
public:
	cScopedTimeProfiler(const char* text);
};
#endif