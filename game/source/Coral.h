#pragma once

#include <GL/glew.h>


#include "cDefines.h"
#include "cVector.h"

extern GLuint postProcessQuad;
extern GLuint postProcessVertex;

class cSoundManager;
class StackWalkerToConsole;
class cSlaveController;
class cAccumulatedTimeProfiler;
class CSteam;
class cParticle;

class Coral
{
	friend class cGame;
	friend void RunGame();
	friend void InitGame();

	float lastDrawTime;
	float lastUpdateTime;
	bool tickedBeforeRender;

	GLuint tempFrameBuffer[3];
	GLuint tempFrameBufferTexture[3];


	bool noSleep;
	bool fullScreen;
	bool lastFullScreen;
	void initFrameBuffers();
	int windowWidth, windowHeight;
	bool gameRunning;
	cSoundManager *soundManager;
	cSlaveController *slaveController;

	int dirtyParticleIndex;
	cVector<cParticle*> dirtyParticles;
#ifdef HAS_STEAM
	CSteam *steam;
#endif
public:
	Coral();
	void init();
	void clear();
	void tick();
	void setNoSleep(bool noSleep)
	{
		this->noSleep = noSleep;
	}

	int getWindowWidth() const
	{
		return windowWidth;
	}

	int getWinodwHeight() const
	{
		return windowHeight;
	}

	void setWindowSize(int width, int height);
	void setFullScreen(bool fullScreen);
	bool isFullScreen() const
	{
		return fullScreen;
	}
	void windowResized(int width, int height);
	void quitGame()
	{
		gameRunning = false;
	}

	static bool isDebuggerPresent();

	cSoundManager* getSoundManager() const
	{
		return soundManager;
	}

	cSlaveController* getSlaveController() const
	{
		return slaveController;
	}

	GLuint getTempFrameBuffer() const
	{
		return tempFrameBuffer[2];
	}

	GLuint getTempFrameBufferTexture() const
	{
		return tempFrameBufferTexture[2];
	}

#ifdef HAS_STEAM
	CSteam* getSteam() const
	{
		return steam;
	}

#endif

	static cAccumulatedTimeProfiler& createAccumulatedTimeProfile(const char *name);
	void clearWindow();

	int getDirtyParticleIndex() const
	{
		return dirtyParticleIndex;
	}
	void removeDirtyParticle(cParticle* particle);
	void addDirtyParticle(cParticle* particle);
};
