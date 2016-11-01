#include "cTools.h"

#include <stdio.h>
#include <fstream>
#include <streambuf>
#include <SDL.h>

#ifdef WINDOWS
#undef APIENTRY
#include "windows.h"
#endif

bool textFileRead(const char *fileName, std::string &data) 
{
	std::ifstream t(fileName);
	if (!t.good()) {
		return false;
	}
	t.seekg(0, std::ios::end);
	data.reserve((unsigned int)t.tellg());
	t.seekg(0, std::ios::beg);

	data.assign((std::istreambuf_iterator<char>(t)),
		std::istreambuf_iterator<char>());
	return true;
}


void sleepMS(int ms) 
{
#ifdef WINDOWS
	Sleep(ms);
#else
	SDL_Delay(ms);
#endif
}


int randInt()
{
	return rand();
}

float randFloat()
{
	return randInt() / (float)RAND_MAX;
}

bool randBool()
{
	return (randInt() & 1) == 0;
}

int randInt(int limit)
{
	return randInt() % limit;
}

int randInt(int begin, int end)
{
	return randInt() % (end - begin + 1) + begin;
}

float randFloat(float limit)
{
	return randFloat() * limit;
}

float randFloat(float begin, float end)
{
	return randFloat() * (end - begin) + begin;
}