#pragma once

class cSlaveWork 
{
	bool done;
	bool cancelled;
	friend class cSlaveController;
	void setIsDone(bool done)
	{
		this->done = done;
	}
	void setCancelled(bool cancelled)
	{
		this->cancelled = cancelled;
	}
public:
	int priority;
	cSlaveWork() 
	{
		priority = 0;
		done = false;
		cancelled = false;
	}
	bool isDone() const
	{
		return done;
	}
	bool isCancelled() const
	{
		return cancelled;
	}
	virtual void runOnSlave() {};
	virtual void runOnMain() {};
};