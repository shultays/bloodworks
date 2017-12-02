#include "cMutex.h"
#include <chrono>

gMutex::gMutex() {
}

gMutex::~gMutex() {
}


void gMutex::waitAndGetMutex() {
	mutex.lock();
}


bool gMutex::waitAndGetMutex(int maxWaitInMS) {
	auto now = std::chrono::steady_clock::now();
	return mutex.try_lock_until(now + std::chrono::milliseconds(maxWaitInMS));
}


bool gMutex::tryToGetMutex() {
	return mutex.try_lock();
}


void gMutex::releaseMutex() {
	mutex.unlock();
}

