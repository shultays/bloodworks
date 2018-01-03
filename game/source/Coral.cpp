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
#include "cSteam.h"

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
	noSleep = false;
	cPackHelper::deleteFolder(TEMP_FOLDER, true, false);
}

bool disableAddParticle = false;
bool renderParticleOnly = false;
bool disableParticleRender = false;
bool dontTickGame = false;

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
		int maxTick = 3;
		while ((t = timer.getRealTime()) - lastUpdateTime >= update_interval && maxTick --> 0)
		{
			ADD_SCOPED_TIME_PROFILER(">>>>  tick  <<<<");

#ifdef SHOW_TIMINGS
			for (int i = 0; i < profilerCount; i++)
			{
				profilers[i].reset();
			}
#endif

			float updateDt = t - lastUpdateTime;
			if (updateDt > update_interval * 2.0f )
			{
				updateDt = update_interval * 2.0f;
			}

			timer.currentTime = updateDt * slowdown + timer.currentTime;
			timer.dt = updateDt * slowdown;
			timer.realDt = updateDt;
			tickedBeforeRender = true;
			if (t - lastUpdateTime < updateDt * 2.0f)
			{
				lastUpdateTime += updateDt;
			}
			else
			{
				lastUpdateTime = t;
			}

			if (input.isKeyPressed(key_1))
			{
				disableAddParticle = !disableAddParticle;
			}
			if (input.isKeyPressed(key_2))
			{
				disableParticleRender = !disableParticleRender;
			}
			if (input.isKeyPressed(key_3))
			{
				renderParticleOnly = !renderParticleOnly;
			}
			if (input.isKeyPressed(key_4))
			{
				dontTickGame = !dontTickGame;
				if (dontTickGame)
				{
					game->setSlowdown(0.0f);
				}
				else
				{
					game->setSlowdown(1.0f);
				}
			}

			slaveController->update();
			debugRenderer.tick(timer.getDt());
			input.preTick();
			if (!dontTickGame)
			{
				game->tickInternal();
			}
			input.tick();
			mapper.check();

#ifdef HAS_STEAM
			steam->tick();
#endif

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

			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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
	if (timeToSleep > 0.01f && noSleep == false) 
	{
		SDL_Delay((Uint32)(timeToSleep * 1000));
	}
	else
	{
		SDL_Delay(0);
	}
}

void Coral::setWindowSize(int width, int height)
{
	int w, h;
	SDL_GetWindowSize(mainWindow, &w, &h);
	if (w != width || h != height)
	{
		SDL_SetWindowSize(mainWindow, width, height);
		SDL_SetWindowPosition(mainWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
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
	static bool present = IsDebuggerPresent() != 0;
	return present;
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

void Coral::clearWindow()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Coral::initFrameBuffers()
{
	out << "Coral::initFrameBuffers\n";
	if (tempFrameBuffer[0] != -1)
	{
		glDeleteTextures(3, tempFrameBufferTexture);
		CHECK_GL_ERROR;
		glDeleteFramebuffers(3, tempFrameBuffer);
		CHECK_GL_ERROR;
	}
	out << "Coral::initFrameBuffers 1\n";

	CHECK_GL_ERROR;
	glGenFramebuffers(3, tempFrameBuffer);
	CHECK_GL_ERROR;
	glGenTextures(3, tempFrameBufferTexture);
	CHECK_GL_ERROR;

	out << "Coral::initFrameBuffers 2\n";
	IntVec2 size = game->getScreenDimensions();

	for (int i = 0; i < 3; i++)
	{
		out << "Coral::initFrameBuffers 3 " << i << "\n";
		glBindFramebuffer(GL_FRAMEBUFFER, tempFrameBuffer[i]);
		CHECK_GL_ERROR;

		out << "Coral::initFrameBuffers 4 " << i << "\n";
		glBindTexture(GL_TEXTURE_2D, tempFrameBufferTexture[i]);
		CHECK_GL_ERROR;
		glTexImage2D(GL_TEXTURE_2D, 0, i < 2 ? GL_RGB : GL_RGBA, size.w, size.h, 0, i < 2 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, 0);
		CHECK_GL_ERROR;

		out << "Coral::initFrameBuffers 5 " << i << "\n";
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		CHECK_GL_ERROR;
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		CHECK_GL_ERROR;

		out << "Coral::initFrameBuffers 6 " << i << "\n";
		glGenerateMipmap(GL_TEXTURE_2D);
		CHECK_GL_ERROR;

		out << "Coral::initFrameBuffers 7 " << i << "\n";

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tempFrameBufferTexture[i], 0);
		CHECK_GL_ERROR;
	}

	out << "Coral::initFrameBuffers 8\n";
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	CHECK_GL_ERROR;
	out << "Coral::initFrameBuffers fin\n";
}

void Coral::init()
{
	out << "Coral::init\n";
	std::string str;
	str = (const char*)glGetString(GL_VENDOR);
	out << "Vendor: " << str << "\n";
	str = (const char*)glGetString(GL_RENDERER);
	out << "Renderer: " <<  str << "\n";
	str = (const char*)glGetString(GL_VERSION);
	out << "OpenGL version: " << str << "\n";
	str = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
	out << "GLSL: " << str << "\n";

	out << "Coral::init 1\n";
	slaveController = new cSlaveController();
	slaveController->startSlaves(3);

	out << "Coral::init 2\n";
	curl_global_init(CURL_GLOBAL_DEFAULT);

	out << "Coral::init 3\n";
	gameRunning = true;
	fullScreen = false;
	lastFullScreen = false;
	SDL_GetWindowSize(mainWindow, &windowWidth, &windowHeight);

	out << "Coral::init 4\n";
	GLfloat vertexData[] =
	{
		-1.0f, 1.0f,
		1.0f,  1.0f,
		1.0f,  -1.0f,
		-1.0f, -1.0f,
	};

	CHECK_GL_ERROR;
	glGenBuffers(1, &postProcessQuad);
	CHECK_GL_ERROR;
	glBindBuffer(GL_ARRAY_BUFFER, postProcessQuad);
	CHECK_GL_ERROR;
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
	CHECK_GL_ERROR;

	out << "Coral::init 5\n";
	timer.init();
	input.init();
	tickedBeforeRender = false;
	lastDrawTime = timer.getTime() - draw_interval;
	lastUpdateTime = timer.getTime() - update_interval;
	out << "Coral::init 6\n";
	initFrameBuffers();

	out << "Coral::init 7\n";
	soundManager = new cSoundManager();

	out << "Coral::init 8\n";
#ifdef HAS_STEAM
	steam = new CSteam();
#endif
	out << "Coral::init fin\n";
}

void Coral::clear()
{
	glDeleteTextures(3, tempFrameBufferTexture);
	glDeleteFramebuffers(3, tempFrameBuffer);
	glDeleteBuffers(1, &postProcessQuad);

#ifdef HAS_STEAM
	SAFE_DELETE(steam);
#endif

	SAFE_DELETE(soundManager);
	SAFE_DELETE(slaveController);
}

