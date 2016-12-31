#pragma once

#define BUCKET_DURATION 6.0f
#define BUCKET_COUNT 10

#include "cGlobals.h"
#include "cSound.h"
#include "sol.h"
#include "Bloodworks.h"

#include <unordered_map>

class OneShotSoundManager
{
	Bloodworks *bloodworks;
	float lastBucketSwitchTime;
	int currentBucket;
	std::unordered_map<int, cSoundSampleShr> buckets[BUCKET_COUNT];
public:
	OneShotSoundManager(Bloodworks *bloodworks)
	{
		this->bloodworks = bloodworks;
		lastBucketSwitchTime = timer.getTime();
		currentBucket = 0;
	}

	~OneShotSoundManager()
	{
		for (int i = 0; i < BUCKET_COUNT; i++)
		{
			buckets[i].clear();
		}
	}

	void addSample(cSoundSampleShr& sample)
	{
		buckets[currentBucket][sample.getUniqueId()] = sample;
	}

	void tick()
	{
		if (timer.getTime() - lastBucketSwitchTime > BUCKET_DURATION)
		{
			lastBucketSwitchTime += BUCKET_DURATION;
			currentBucket++;
			if (currentBucket >= BUCKET_COUNT)
			{
				currentBucket = 0;
			}
			buckets[currentBucket].clear();
		}
	}

	cSoundHandle playSample(sol::table& args)
	{
		cSoundSampleWithParams sample = createSample(args);
		cSoundHandle handle;
		if (args["position"])
		{
			float volume = sample.getVolume() * bloodworks->getVolumeMultiplier(args["position"].get<Vec2>());
			if (volume > 0.0f)
			{
				sample.setVolume(volume);
			}
			else
			{
				return cSoundHandle();
			}
		}
		handle = sample.play();
		if (!(bool)args["uiSound"])
		{
			bloodworks->addGameSound(handle);
		}
		return handle;
	}

	cSoundSampleWithParams createSample(sol::table& args)
	{
		cSoundSampleWithParams sample;
		sample.loadSample(args);
		addSample(sample.getSample());
		return sample;
	}
};