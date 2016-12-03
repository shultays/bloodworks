#include "Coral.h"
#include <SDL.h>
#include <GL/glew.h>

#include "cGlobals.h"
#include "cGame.h"
#include "cTools.h"

extern SDL_Window *mainWindow;

Coral::Coral()
{
	lastDrawTime = 0.0f;
	lastUpdateTime = 0.0f;
}

void Coral::tick()
{
	float slowdown = game->getSlowdown();

	float t;
	while ((t = timer.getRealTime()) - lastUpdateTime >= update_interval)
	{
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
		debugRenderer.tick(timer.getDt());
		game->tickInternal();
		input.tick();
	}

	if ((t = timer.getRealTime()) - lastDrawTime >= draw_interval * 0.95f)
	{
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
	
	t = timer.getRealTime();
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

void Coral::init()
{
	timer.init();
	input.init();
	tickedBeforeRender = false;
	lastDrawTime = timer.getTime() - draw_interval;
	lastUpdateTime = timer.getTime() - update_interval;
}