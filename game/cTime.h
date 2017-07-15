#pragma once

class cTime 
{
friend class Coral;
	void init();

	float currentTime;
	float renderTime;
	float dt;
	float realDt;
	float renderDt;
public:
	float getTime()  const
	{
		return currentTime;
	}

	float getDt() const
	{
		return dt;
	}

	float getNonSlowedDt() const
	{
		return realDt;
	}

	float getRender_dt()  const
	{
		return renderDt;
	}

	float getRealTime();
	float getRenderTime()
	{
		return renderTime;
	}
};
