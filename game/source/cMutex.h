#pragma once

#include <mutex>

class gMutex {
	std::timed_mutex mutex;

public:
	gMutex();
	~gMutex();

	void waitAndGetMutex();
	bool waitAndGetMutex(int maxWaitInMS);
	bool tryToGetMutex();
	void releaseMutex();
};

