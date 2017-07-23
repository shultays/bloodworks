#include "cSlave.h"

int slaveIndex = 0;

void cSlave::startSlaveThread(cSlaveController* controller) 
{
	assert(!sharedData);
	ThreadData* data = new ThreadData;
	data->slave = this;
	data->slaveIndex = ++slaveIndex;
	data->freed = false;
	data->workToDo = nullptr;
	data->controller = controller;
	this->controller = controller;
	sharedData = data;

	startSlaveThreadNative();
}

void cSlaveController::startSlaves(int slaveCount) 
{
	this->slaveCount = slaveCount;
	slaves = new cSlave[slaveCount];
	for (int i = 0; i < slaveCount; i++) 
	{
		slaves[i].startSlaveThread(this);
	}
}

void cSlaveController::freeSlaves() 
{
	for (int i = 0; i < slaveCount; i++) 
	{
		slaves[i].freeSlaveThread();
	}
	delete[] slaves;
	slaveCount = 0;
}

void cSlaveController::update() 
{
	std::vector<cSlaveWork*> works;
	controllerMutex.waitAndGetMutex();
	while (workForMainQueue.size() > 0) 
	{
		cSlaveWork* work = workForMainQueue.front();
		workForMainQueue.pop();
		works.push_back(work);
	}
	controllerMutex.releaseMutex();

	for (unsigned i = 0; i < works.size(); i++) 
	{
		assert(works[i]->isCancelled() == false);
		works[i]->runOnMain();
		works[i]->setIsDone(true);
	}
}

cSlaveWork* cSlave::getNextJobForSlave() 
{
	return controller->getNextJobForSlave();
}

void cSlave::slaveWorkDone(cSlaveWork* work) 
{
	controller->addMainWork(work);
}

void cSlaveController::addWork(cSlaveWork* work) 
{
	controllerMutex.waitAndGetMutex();
	work->setIsDone(false);
	work->setCancelled(false);
	workForSlavesQueue.push(work);
	controllerMutex.releaseMutex();
}