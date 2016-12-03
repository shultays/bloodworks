#include "cTime.h"
#include <SDL.h>


void cTime::init()
{
	renderTime = currentTime = getRealTime();
	dt = 0.0f;
	renderDt = 0.0f;
	realDt = 0.0f;
}

float cTime::getRealTime()
{
	return SDL_GetTicks() / 1000.0f;
}