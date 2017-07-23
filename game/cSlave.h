#pragma once

#include "cTools.h"
#include "cMutex.h"
#include "cSlaveWork.h"
#include <queue>

class cSlave;
class cSlaveController;

class Compare 
{
public:
	bool operator() (const cSlaveWork* slave0, const cSlaveWork* slave1) {
		return slave0->priority < slave1->priority;
	}
};

struct ThreadData 
{
	cSlave* slave;
	int slaveIndex;
	bool freed;
	cSlaveWork* workToDo;
	cSlaveController* controller;
};

class cSlave 
{
	ThreadData* sharedData;
	void startSlaveThreadNative();
	cSlaveController* controller;
public:
	cSlave() 
	{
		sharedData = nullptr;
		this->controller = nullptr;
	}

	void startSlaveThread(cSlaveController* controller);

	void freeSlaveThread() 
	{
		assert(sharedData);
		sharedData->freed = true;
		sharedData = nullptr;
	}

	bool isWorking() 
	{
		return sharedData && sharedData->workToDo != nullptr;
	}

	void slaveWorkDone(cSlaveWork* work);

	cSlaveWork* getNextJobForSlave();
};

class cSlaveController 
{
	int slaveCount;
	cSlave* slaves;
	std::priority_queue<cSlaveWork*, std::vector<cSlaveWork*>, Compare> workForSlavesQueue;

	std::queue<cSlaveWork*> workForMainQueue;
	gMutex controllerMutex;
	friend class cSlave;

	cSlaveWork* getNextJobForSlave() 
	{
		if (workForSlavesQueue.size()) 
		{
			cSlaveWork* work = nullptr;
			controllerMutex.waitAndGetMutex();
			if (workForSlavesQueue.size()) 
			{
				work = workForSlavesQueue.top();
				workForSlavesQueue.pop();
			}
			controllerMutex.releaseMutex();
			return work;
		}
		return nullptr;
	}

	void addMainWork(cSlaveWork* work) 
	{
		controllerMutex.waitAndGetMutex();
		if (work->isCancelled() == false)
		{
			workForMainQueue.push(work);
		}
		controllerMutex.releaseMutex();
	}
public:

	void startSlaves(int slaveCount);
	void freeSlaves();
	void addWork(cSlaveWork* work);
	void update();
	void cancelWork(cSlaveWork* work)
	{
		controllerMutex.waitAndGetMutex();
		assert(work->isDone() == false);
		work->setCancelled(true);
		controllerMutex.releaseMutex();
	}
	bool hasJobForSlaves() 
	{
		return workForSlavesQueue.size() > 0;
	}
};

