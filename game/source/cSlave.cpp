#include "cSlave.h"

int slaveIndex = 0;


void SlaveThreadFunc(ThreadData* data)
{
	while (data->freed == false)
	{
		if (!data->workToDo)
		{
			data->workToDo = data->slave->getNextJobForSlave();
		}

		if (data->workToDo)
		{
			data->workToDo->runOnSlave();
			data->slave->slaveWorkDone(data->workToDo);
			data->workToDo = nullptr;
		}

		sleepMS(10);
	}

	SAFE_DELETE(data);
}

void cSlave::startSlaveThread(cSlaveController* controller) 
{
	assert(!sharedData);
	ThreadData* data = new ThreadData;
	data->slave = this;

	data->freed = false;
	data->workToDo = nullptr;
	data->controller = controller;
	this->controller = controller;
	sharedData = data;

	data->thread = std::thread(SlaveThreadFunc, sharedData);
}

cSlaveController::cSlaveController()
{

}

cSlaveController::~cSlaveController()
{
	freeSlaves();
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
	cVector<cSlaveWork*> works;
	controllerMutex.waitAndGetMutex();
	while (workForMainQueue.size() > 0) 
	{
		cSlaveWork* work = workForMainQueue.front();
		workForMainQueue.pop();
		works.push_back(work);
	}
	controllerMutex.releaseMutex();

	for (int i = 0; i < works.size(); i++) 
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



