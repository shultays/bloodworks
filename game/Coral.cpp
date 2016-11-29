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
	float t;
	while ((t = timer.getRealTime()) - lastUpdateTime >= update_interval)
	{
		if (t - lastUpdateTime < update_interval * 2.0f)
		{
			timer.dt = update_interval;
			lastUpdateTime += update_interval;
		}
		else
		{
			timer.dt = t - lastUpdateTime;
			lastUpdateTime = t;
		}
		timer.currentTime = lastUpdateTime;
		debugRenderer.tick(timer.getDt());
		game->tickInternal();
		input.tick();
	}

	if ((t = timer.getRealTime()) - lastDrawTime >= draw_interval * 0.9f)
	{
		timer.renderDt = t - lastDrawTime;
		timer.renderTime = lastDrawTime = t;
		glClear(GL_COLOR_BUFFER_BIT);

		game->renderInternal();

		SDL_GL_SwapWindow(mainWindow);
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

	lastDrawTime = timer.getTime() - draw_interval;
	lastUpdateTime = timer.getTime() - update_interval;
}