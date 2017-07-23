#include "cMutex.h"

#undef APIENTRY
#include "windows.h"

gMutex::gMutex() {
	mutex = CreateMutex(NULL, FALSE, NULL);
}

gMutex::~gMutex() {
	CloseHandle(mutex);
}


void gMutex::waitAndGetMutex() {
	WaitForSingleObject(mutex, INFINITE);
}


bool gMutex::waitAndGetMutex(int maxWaitInMS) {
	return WaitForSingleObject(mutex, maxWaitInMS) == WAIT_OBJECT_0;
}


bool gMutex::tryToGetMutex() {
	return WaitForSingleObject(mutex, 0) == WAIT_OBJECT_0;
}


void gMutex::releaseMutex() {
	ReleaseMutex(mutex);
}

