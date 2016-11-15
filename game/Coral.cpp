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
	timer.tick();

	float t = timer.getTime();


	if (t - lastUpdateTime >= update_interval) 
	{
		timer.dt = update_interval;
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

	if (t - lastDrawTime >= draw_interval) 
	{
		timer.renderDt = t - lastDrawTime;
		lastDrawTime = t;
		glClear(GL_COLOR_BUFFER_BIT);

		game->renderInternal();

		SDL_GL_SwapWindow(mainWindow);
	}
	
	t = timer.getRealTime();
	float timeToSleep = min(update_interval - (t - lastUpdateTime), draw_interval - (t - lastDrawTime));
	if (timeToSleep > 0.0f) 
	{
		SDL_Delay((Uint32)(timeToSleep * 1000));
	}
}

void Coral::init()
{
	timer.init();
	input.init();

	lastDrawTime = timer.getTime() - draw_interval;
	lastUpdateTime = timer.getTime() - update_interval;
}