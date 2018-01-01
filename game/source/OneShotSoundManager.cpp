#include "OneShotSoundManager.h"
#include "Bloodworks.h"

void OneShotSoundManager::tick()
{
	if (bloodworks->isFirstTick())
	{
		out << "OneShotSoundManager::tick\n";
	}
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

cSoundHandle OneShotSoundManager::playSample(sol::table& args)
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

cSoundSampleWithParams OneShotSoundManager::createSample(sol::table& args)
{
	cSoundSampleWithParams sample;
	sample.loadSample(args);
	addSample(sample.getSample());
	return sample;
}
