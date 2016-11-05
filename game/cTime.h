#pragma once


class cTime 
{
friend class Coral;
	void init();

	float currentTime;
	float dt;
	float renderDt;

public:
	float getTime() 
	{
		return currentTime;
	}

	float getDt() 
	{
		return dt;
	}

	float getRender_dt() 
	{
		return renderDt;
	}

	void tick();
	float getRealTime();
};
