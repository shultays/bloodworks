#pragma once

#include <string>

#include "json.h"
#include "sol.h"
#include "cResources.h"


class cSoundManager;
class Wav;

namespace SoLoud {
	class Wav;
}

class cSoundHandle
{
	friend class cSoundSample;
public:
	int handle;
	cSoundHandle()
	{
		handle = -1;
	}

	void stop();
	void setLooped(bool looped);
	void setVolume(float volume);
	void setPan(float volume);
	void setSpeed(float volume);
	void resume();
	void pause();
	bool isFinished();

	bool isValid() const
	{
		return handle != -1;
	}

	void clear()
	{
		handle = -1;
	}

};
/*
class cSoundHandleGroup : public cSoundHandle
{
public:
	cSoundHandleGroup();
	~cSoundHandleGroup();
	void addHandle(cSoundHandle& handle);
	void init();
	bool isEmpty();
};
*/
class cSoundSample
{
	friend class cSoundSampleWithParams;
	std::string file;
	SoLoud::Wav *sample;
	cSoundManager* soundManager;
public:
	cSoundSample(cSoundManager* soundManager);
	void loadSample(const std::string& file);
	cSoundHandle play(float volume = -1.0f, float pan = 0.0f);
	~cSoundSample();
	const std::string& getName() const;
};

class cSoundSampleWithParams
{
	cSoundSampleShr sample;
	float volume;
	float pan;
	float speed;
	bool looped;

public:
	cSoundSampleWithParams()
	{
		volume = speed = 1.0f;
		pan = 0.0f;
		looped = false;
	}

	~cSoundSampleWithParams()
	{
		sample = nullptr;
	}

	void loadSample(nlohmann::json& j);
	void loadSample(sol::table& t);

	cSoundHandle play();

	bool isValid() const
	{
		return sample != nullptr;
	}

	void setLooped(bool looped)
	{
		this->looped = true;
	}

	cSoundSampleShr getSample() const
	{
		return sample;
	}

	float getVolume() const
	{
		return volume;
	}

	float getPan() const
	{
		return pan;
	}

	float getSpeed() const
	{
		return speed;
	}

	void setVolume(float volume)
	{
		this->volume = volume;
	}

	void setPan(float pan)
	{
		this->pan = pan;
	}

	void setSpeed(float speed)
	{
		this->speed = speed;
	}
};