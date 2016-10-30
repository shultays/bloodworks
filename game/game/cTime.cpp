#include "cTime.h"
#include <SDL.h>


void cTime::init()
{
	currentTime = getRealTime();
	dt = 0.0f;
	fixedDt = 0.0f;
}

void cTime::tick()
{
	float newTime = getRealTime();
	dt = newTime - currentTime;
	currentTime = newTime;
}


float cTime::getRealTime()
{
	return SDL_GetTicks() / 1000.0f;
}