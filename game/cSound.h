#pragma once

#include <string>

#include "json.h"
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

	bool isValid() const
	{
		return handle != -1;
	}

	void clear()
	{
		handle = -1;
	}
};

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

	cSoundHandle play();
	bool isValid() const
	{
		return sample != nullptr;
	}

	void setLooped(bool looped)
	{
		this->looped = true;
	}
};