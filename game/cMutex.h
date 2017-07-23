#pragma once

class gMutex {
	void* mutex;

public:
	gMutex();
	~gMutex();

	void waitAndGetMutex();
	bool waitAndGetMutex(int maxWaitInMS);
	bool tryToGetMutex();
	void releaseMutex();
};

