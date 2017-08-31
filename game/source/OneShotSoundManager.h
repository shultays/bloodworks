#pragma once

#define BUCKET_DURATION 6.0f
#define BUCKET_COUNT 10

#include "cGlobals.h"
#include "cSound.h"
#include "sol.h"

class Bloodworks;

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

	void tick();

	cSoundHandle playSample(sol::table& args);

	cSoundSampleWithParams createSample(sol::table& args);
};