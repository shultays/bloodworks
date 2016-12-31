
#include "cSoundManager.h"
#include "cSound.h"
#include "soloud.h"
#include "soloud_wav.h"
#include "cTools.h"
#include "cGlobals.h"

using namespace SoLoud;

cSoundManager::cSoundManager()
{
	soloud = new SoLoud::Soloud();
	soloud->init();
	soloud->setMaxActiveVoiceCount(128);
}

cSoundManager::~cSoundManager()
{
	soloud->deinit(); // Clean up!
	SAFE_DELETE(soloud);
}

cSoundSample::cSoundSample(cSoundManager* soundManager)
{
	this->soundManager = soundManager;
	this->sample = nullptr;
}

void cSoundSample::loadSample(const std::string& file)
{
	this->file = file;
	sample = new SoLoud::Wav();
	int ret = sample->load(file.c_str());
	assert(ret == 0);
}

cSoundHandle cSoundSample::play(float volume, float pan)
{
	cSoundHandle handle;
	handle.handle = soundManager->soloud->play(*sample, volume, pan);
	return handle;
}

void cSoundHandle::stop()
{
	if (handle != -1)
	{
		coral.getSoundManager()->soloud->stop(handle);
	}
}

void cSoundHandle::setLooped(bool looped)
{
	if (handle != -1)
	{
		coral.getSoundManager()->soloud->setLooping(handle, looped);
	}
}
void cSoundHandle::setVolume(float volume)
{
	if (handle != -1)
	{
		coral.getSoundManager()->soloud->setVolume(handle, volume);
	}
}

void cSoundHandle::setPan(float pan)
{
	if (handle != -1)
	{
		coral.getSoundManager()->soloud->setPan(handle, pan);
	}
}

void cSoundHandle::setSpeed(float speed)
{
	if (handle != -1)
	{
		coral.getSoundManager()->soloud->setRelativePlaySpeed(handle, speed);
	}
}

cSoundSample::~cSoundSample()
{
	SAFE_DELETE(sample);
}

const std::string& cSoundSample::getName() const
{
	return file;
}

void cSoundSampleWithParams::loadSample(nlohmann::json& j)
{
	if (j.is_string())
	{
		sample = resources.getSoundSample(j.get<std::string>());
	}
	else
	{
		sample = resources.getSoundSample(j["path"].get<std::string>());
		if (j.count("volume"))
		{
			volume = j["volume"].get<float>();
		}
		if (j.count("pan"))
		{
			pan = j["pan"].get<float>();
		}
		if (j.count("speed"))
		{
			speed = j["speed"].get<float>();
		}
		if (j.count("looped"))
		{
			looped = j["looped"].get<bool>();
		}
	}
}

cSoundHandle cSoundSampleWithParams::play()
{
	cSoundHandle h = sample->play(volume, pan);
	if (speed != 1.0f)
	{
		sample->soundManager->getSoloud()->setRelativePlaySpeed(h.handle, speed);
	}
	if (looped)
	{
		sample->soundManager->getSoloud()->setLooping(h.handle, true);
	}
	return h;
}
