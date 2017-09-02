#include "cSlave.h"

#undef APIENTRY
#include "windows.h"

DWORD WINAPI SlaveThreadFunc(LPVOID lpParam) 
{
	ThreadData* data = (ThreadData*)lpParam;
	out << "slave started " << data->slaveIndex << "\n";
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
	return 0;
}

void cSlave::startSlaveThreadNative() 
{
	DWORD threadID;
	CreateThread(NULL, 0, SlaveThreadFunc, sharedData, 0, &threadID);
}


