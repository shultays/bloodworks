#include "cInput.h"
#include "cGlobals.h"
#include <stdio.h>
#include <string.h>

void cInput::init()
{
	memset(keyStates, 0, sizeof(keyStates));
	memset(prevKeyStates, 0, sizeof(prevKeyStates));
	memset(keyTime, 0, sizeof(keyTime));
}

void cInput::pressKey(int key)
{
	keyStates[key] = 1;
	keyTime[key] = time.getTime();
}

void cInput::tick()
{
	memcpy(prevKeyStates, keyStates, sizeof(prevKeyStates));
}

void cInput::releaseKey(int key)
{
	keyStates[key] = 0;
	keyTime[key] = time.getTime();
}