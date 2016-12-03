#pragma once


class cTime 
{
friend class Coral;
	void init();

	float currentTime;
	float renderTime;
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

	float getRealTime();
	float getRenderTime()
	{
		return renderTime;
	}
	float realDt;
};
