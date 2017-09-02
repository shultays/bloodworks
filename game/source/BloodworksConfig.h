#pragma once

#include "cPersistent.h"

class BloodworksConfig : protected cPersistent
{
	bool gore;
	bool screenShake;
	bool lockCrosshair;
	bool autoOpenLevelupPopup;

	float sensitivity;

	bool fullScreen;
	bool vSync;
	float volume;
	float musicVolume;

	int windowWidth;
	int windowHeight;

	int crashAutoSendState;
public:
	BloodworksConfig()
	{
		setFileBackup("config.txt");

		gore = getBool("gore", true);
		screenShake = getBool("screenShake", true);
		lockCrosshair = getBool("lockCrosshair", true);
		autoOpenLevelupPopup = getBool("autoOpenLevelupPopup", true);

		sensitivity = getInt("sensitivity", 50) / 100.0f;

		volume = getInt("volume", 50) / 100.0f;
		musicVolume = getInt("musicVolume", 50) / 100.0f;

		fullScreen = getBool("fullScreen", true);
		vSync = getBool("vSync", true);

		windowWidth = getInt("windowWidth", 800);
		windowHeight = getInt("windowHeight", 600);

		crashAutoSendState = getInt("crashAutoSendState", 0);
	}

	bool getGore() const
	{
		return gore;
	}
	void setGore(bool gore)
	{
		this->gore = gore;
		set("gore", gore);
	}

	bool getScreenShake() const
	{
		return screenShake;
	}
	void setScreenShake (bool screenShake)
	{
		this->screenShake = screenShake;
		set("screenShake", screenShake);
	}

	bool getLockCrosshair() const
	{
		return lockCrosshair;
	}
	void setLockCrosshair(bool lockCrosshair)
	{
		this->lockCrosshair = lockCrosshair;
		set("lockCrosshair", lockCrosshair);
	}

	bool getAutoOpenLevelupPopup() const
	{
		return autoOpenLevelupPopup;
	}
	void setAutoOpenLevelupPopup(bool autoOpenLevelupPopup)
	{
		this->autoOpenLevelupPopup = autoOpenLevelupPopup;
		set("autoOpenLevelupPopup", autoOpenLevelupPopup);
	}


	float getSensitivity() const
	{
		return sensitivity;
	}
	void setSensitivity(float sensitivity)
	{
		this->sensitivity = sensitivity;
		set("sensitivity", (int)round(sensitivity * 100));
	}

	float getVolume() const
	{
		return volume;
	}
	void setVolume(float volume)
	{
		this->volume = volume;
		set("volume", (int)round(volume * 100));
	}

	float getMusicVolume() const
	{
		return musicVolume;
	}
	void setMusicVolume(float musicVolume)
	{
		this->musicVolume = musicVolume;
		set("musicVolume", (int)round(musicVolume * 100));
	}

	bool getFullScreen() const
	{
		return fullScreen;
	}
	void setFullScreen(bool fullScreen)
	{
		this->fullScreen = fullScreen;
		set("fullScreen", fullScreen);
	}

	bool getVSync() const
	{
		return vSync;
	}
	void setVSync(bool vSync)
	{
		this->vSync = vSync;
		set("vSync", vSync);
	}


	int getWindowWidth() const
	{
		return windowWidth;
	}
	int getWindowHeight() const
	{
		return windowHeight;
	}

	void setWindowWidth(int windowWidth)
	{
		this->windowWidth = windowWidth;
		set("windowWidth", windowWidth);
	}
	void setWindowHeight(int windowHeight)
	{
		this->windowHeight = windowHeight;
		set("windowHeight", windowHeight);
	}


	void setCrashAutoSendState(int crashAutoSendState)
	{
		this->crashAutoSendState = crashAutoSendState;
		set("crashAutoSendState", crashAutoSendState);
	}
	int getCrashAutoSendState() const
	{
		return crashAutoSendState;
	}



	void check()
	{
		cPersistent::check();
	}
};