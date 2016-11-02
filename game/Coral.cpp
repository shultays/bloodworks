#include "Coral.h"
#include <SDL.h>
#include <GL/glew.h>

#include "cGlobals.h"
#include "cTools.h"

extern SDL_Window *mainWindow;

Coral::Coral()
{
	lastDrawTime = 0.0f;
	lastUpdateTime = 0.0f;
}

void Coral::tick()
{
	time.tick();

	float t = time.getTime();

	time.dt = t - lastDrawTime;

	if (t - lastUpdateTime >= update_interval) {
		lastUpdateTime = t;
		game->tick(time.getDt());
		input.tick();
	}

	if (t - lastDrawTime >= draw_interval) {
		glClear(GL_COLOR_BUFFER_BIT);

		game->render();
		lastDrawTime = t;

		SDL_GL_SwapWindow(mainWindow);
	}

	t = time.getRealTime();
	float timeToSleep = min(update_interval - (t - lastUpdateTime), draw_interval - (t - lastDrawTime));
	if (timeToSleep > 0.0f) 
	{
		SDL_Delay((Uint32)(timeToSleep * 1000));
	}
}

void Coral::init()
{
	time.init();
	input.init();

	lastDrawTime = time.getTime() - draw_interval;
	lastUpdateTime = time.getTime() - update_interval;
}