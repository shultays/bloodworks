#pragma once

#ifndef BloodworksControls_h__
#define BloodworksControls_h__


#include "cInput.h"
#include "KeyMapper.h"

enum class GameKey : MappedKey
{
	Left = 0,
	Right,
	Up,
	Down,
	Attack,
	Attack2,
	Special,
	Reload,
	LevelUp,
	Pause,
	SwapGuns,
	ShowHints,
	Select,
	Back,
	Fullscreen,
	EndStatic
};

class BloodworksControls
{
public:
	static void init();
};

#endif // BloodworksControls_h__