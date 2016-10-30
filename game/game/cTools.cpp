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
