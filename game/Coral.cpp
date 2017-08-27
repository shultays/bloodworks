#include "Coral.h"
#include <SDL.h>
#include <GL/glew.h>

#include "cGlobals.h"
#include "cGame.h"
#include "cTools.h"
#include "cSoundManager.h"
#include "cSlave.h"
#include "cPackHelper.h"
#include "cTimeProfiler.h"

#define CURL_STATICLIB 
#include <curl/curl.h>

#ifdef _WIN32
#include "Windows.h"
#endif
GLuint postProcessQuad;

extern SDL_Window *mainWindow;

#ifdef SHOW_TIMINGS
cAccumulatedTimeProfiler profilers[512];
int profilerCount = 0;
#endif

Coral::Coral()
{
	lastDrawTime = 0.0f;
	lastUpdateTime = 0.0f;

	tempFrameBuffer[0] = -1;

	cPackHelper::deleteFolder(TEMP_FOLDER, true, false);
}

void Coral::tick()
{
	if (lastFullScreen != fullScreen)
	{
		lastFullScreen = fullScreen;
		if (fullScreen)
		{
			SDL_SetWindowFullscreen(mainWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
			// SDL_SetWindowSize(mainWindow, 1200, 800);
		}
		else
		{
			SDL_SetWindowFullscreen(mainWindow, 0);
			// SDL_SetWindowSize(mainWindow, 800, 600);
		}
	}
	{
		ADD_SCOPED_TIME_PROFILER("======== frame ========");

		float slowdown = game->getSlowdown();
		float t;
		int maxTick = 4;
		while ((t = timer.getRealTime()) - lastUpdateTime >= update_interval && maxTick --> 0)
		{
			ADD_SCOPED_TIME_PROFILER(">>>>  tick  <<<<");

#ifdef SHOW_TIMINGS
			for (int i = 0; i < profilerCount; i++)
			{
				profilers[i].reset();
			}
#endif
			timer.currentTime = update_interval * slowdown + timer.currentTime;
			timer.dt = update_interval * slowdown;
			timer.realDt = update_interval;
			tickedBeforeRender = true;
			if (t - lastUpdateTime < update_interval * 2.0f)
			{
				lastUpdateTime += update_interval;
			}
			else
			{
				lastUpdateTime = t;
			}
			slaveController->update();
			debugRenderer.tick(timer.getDt());
			game->tickInternal();
			input.tick();
			mapper.check();

#ifdef SHOW_TIMINGS
			for (int i = 0; i < profilerCount; i++)
			{
				profilers[i].printResult();
			}
#endif
		}

		if ((t = timer.getRealTime()) - lastDrawTime >= draw_interval * 0.95f)
		{
			ADD_SCOPED_TIME_PROFILER(">>>> render <<<<");
			if (tickedBeforeRender)
			{
				tickedBeforeRender = false;
				timer.renderDt = (t - lastUpdateTime) * slowdown + timer.currentTime - timer.renderTime;
				timer.renderTime += timer.renderDt;
			}
			else
			{
				timer.renderDt = draw_interval * slowdown;
				timer.renderTime += timer.renderDt;
			}

			glClear(GL_COLOR_BUFFER_BIT);

			game->renderInternal();

			SDL_GL_SwapWindow(mainWindow);
			lastDrawTime = timer.getRealTime();
		}
	}
#ifdef SHOW_TIMINGS
	if (input.isKeyDown(key_f12))
	{
		doBreak();
	}
#endif
	resources.tick();
	float t = timer.getRealTime();
	float timeToSleep = min(update_interval - (t - lastUpdateTime), draw_interval - (t - lastDrawTime));
	if (timeToSleep > 0.01f) 
	{
		SDL_Delay((Uint32)(timeToSleep * 1000));
	}
	else
	{
		SDL_Delay(1);
	}
}

void Coral::setFullScreen(bool fullScreen)
{
	if (this->fullScreen != fullScreen)
	{
		this->fullScreen = fullScreen;
	}
}

void Coral::windowResized(int width, int height)
{
	this->windowWidth = width;
	this->windowHeight = height;
	glClear(GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, width, height);
	initFrameBuffers();
}

bool Coral::isDebuggerPresent()
{
#ifdef _WIN32
	return IsDebuggerPresent() != 0;
#endif
	return false;
}

cAccumulatedTimeProfiler& Coral::createAccumulatedTimeProfile(const char *name)
{
#ifdef SHOW_TIMINGS
	profilers[profilerCount] = cAccumulatedTimeProfiler(name);
	return profilers[profilerCount++];
#else
	static cAccumulatedTimeProfiler dummy;
	return dummy;
#endif
}

void Coral::initFrameBuffers()
{
	if (tempFrameBuffer[0] != -1)
	{
		glDeleteTextures(3, tempFrameBufferTexture);
		glDeleteFramebuffers(3, tempFrameBuffer);
	}

	glGenFramebuffers(3, tempFrameBuffer);
	glGenTextures(3, tempFrameBufferTexture);

	IntVec2 size = game->getScreenDimensions();

	for (int i = 0; i < 3; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, tempFrameBuffer[i]);

		glBindTexture(GL_TEXTURE_2D, tempFrameBufferTexture[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, i < 2 ? GL_RGB : GL_RGBA, size.w, size.h, 0, i < 2 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, 0);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glGenerateMipmap(GL_TEXTURE_2D);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, tempFrameBufferTexture[i], 0);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Coral::init()
{
	slaveController = new cSlaveController();
	slaveController->startSlaves(3);

	curl_global_init(CURL_GLOBAL_DEFAULT);

	gameRunning = true;
	fullScreen = false;
	lastFullScreen = false;
	SDL_GetWindowSize(mainWindow, &windowWidth, &windowHeight);

	GLfloat vertexData[] =
	{
		-1.0f, 1.0f,
		1.0f,  1.0f,
		1.0f,  -1.0f,
		-1.0f, -1.0f,
	};

	glGenBuffers(1, &postProcessQuad);
	glBindBuffer(GL_ARRAY_BUFFER, postProcessQuad);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

	timer.init();
	input.init();
	tickedBeforeRender = false;
	lastDrawTime = timer.getTime() - draw_interval;
	lastUpdateTime = timer.getTime() - update_interval;
	initFrameBuffers();

	soundManager = new cSoundManager();
}

void Coral::clear()
{
	glDeleteTextures(3, tempFrameBufferTexture);
	glDeleteFramebuffers(3, tempFrameBuffer);
	glDeleteBuffers(1, &postProcessQuad);

	SAFE_DELETE(soundManager);
	SAFE_DELETE(slaveController);
}

