
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

void cSoundManager::clearAllSounds()
{
	soloud->stopAll();
}

void cSoundManager::setGlobalVolume(float globalVolume)
{
	soloud->setGlobalVolume(globalVolume);
}

float cSoundManager::getGlobalVolume() const
{
	return soloud->getGlobalVolume();
}

extern int totalResource;

cSoundSample::cSoundSample(cSoundManager* soundManager)
{
	this->soundManager = soundManager;
	this->sample = nullptr;
	totalResource++;
}

void cSoundSample::loadSample(const std::string& file)
{
	out << "loading " << file << "\n";
	this->file = file;
	fixFilePath(this->file);
	sample = new SoLoud::Wav();
	int ret = sample->load(this->file.c_str());
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
		coral.getSoundManager()->soloud->setVolume(handle, volume * coral.getSoundManager()->getGlobalVolume());
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
		coral.getSoundManager()->soloud->setRelativePlaySpeed(handle, speed * baseSpeed);
	}
}

void cSoundHandle::pause()
{
	if (handle != -1)
	{
		coral.getSoundManager()->soloud->setPause(handle, true);
	}
}

bool cSoundHandle::isFinished()
{
	if (handle != -1)
	{
		return coral.getSoundManager()->soloud->isValidVoiceHandle(handle) == false;
	}
	return true;
}

void cSoundHandle::resume()
{
	if (handle != -1)
	{
		coral.getSoundManager()->soloud->setPause(handle, false);
	}
}
/*
cSoundHandleGroup::cSoundHandleGroup()
{
	handle = -1;
}

cSoundHandleGroup::~cSoundHandleGroup()
{
	coral.getSoundManager()->getSoloud()->destroyVoiceGroup(handle);
}

void cSoundHandleGroup::init()
{
	handle = coral.getSoundManager()->getSoloud()->createVoiceGroup();
}

bool cSoundHandleGroup::isEmpty()
{
	return coral.getSoundManager()->getSoloud()->isVoiceGroupEmpty(handle);
}

void cSoundHandleGroup::addHandle(cSoundHandle& handle)
{
	coral.getSoundManager()->getSoloud()->addVoiceToGroup(this->handle, handle.handle);
}
*/
cSoundSample::~cSoundSample()
{
#ifdef DEBUG
	out << "unloading " << file << "\n";
#endif
	totalResource--;
	SAFE_DELETE(sample);
}

const std::string& cSoundSample::getName() const
{
	return file;
}

void cSoundSampleWithParams::loadSample(nlohmann::json& j, const DirentHelper::File& file)
{
	if (j.is_string())
	{
		sample = resources.getSoundSample(file.folder + j.get<std::string>());
	}
	else
	{
		sample = resources.getSoundSample(file.folder + j["path"].get<std::string>());
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


void cSoundSampleWithParams::loadSample(sol::table& t)
{
	sample = resources.getSoundSample(t["path"].get<std::string>());
	if (t["volume"])
	{
		volume = t["volume"].get<float>();
	}
	if (t["pan"])
	{
		pan = t["pan"].get<float>();
	}
	if (t["speed"])
	{
		speed = t["speed"].get<float>();
	}
	if (t["looped"])
	{
		looped = t["looped"].get<bool>();
	}
}

cSoundHandle cSoundSampleWithParams::play()
{
	cSoundHandle h = sample->play(volume, pan);
	if (speed != 1.0f)
	{
		h.setBaseSpeed(speed);
		sample->soundManager->getSoloud()->setRelativePlaySpeed(h.handle, speed);
	}
	if (looped)
	{
		sample->soundManager->getSoloud()->setLooping(h.handle, true);
	}
	return h;
}
