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

public:
	cSoundSampleWithParams()
	{
		volume = speed = 1.0f;
		pan = 0.0f;
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
};