#pragma once


class cTime 
{
friend class Coral;
	void init();

	float currentTime;
	float dt;
	float fixedDt;

public:
	float getTime() 
	{
		return currentTime;
	}

	float getDt() 
	{
		return dt;
	}

	float getFixed_dt() 
	{
		return fixedDt;
	}

	void tick();
	float getRealTime();
};
