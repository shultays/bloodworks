#pragma once

#include <GL/glew.h>

const float update_interval = 0.01f;
const float draw_interval = 1.0f / 60.0f;

extern GLuint postProcessQuad;

class cSoundManager;
class StackWalkerToConsole;
class cSlaveController;
class cAccumulatedTimeProfiler;

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

	bool fullScreen;
	bool lastFullScreen;
	void initFrameBuffers();
	int windowWidth, windowHeight;
	bool gameRunning;
	cSoundManager *soundManager;
	cSlaveController *slaveController;

public:
	Coral();
	void init();
	void clear();
	void tick();
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

	bool isDebuggerPresent();

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

	static cAccumulatedTimeProfiler& createAccumulatedTimeProfile(const char *name);
};
