#pragma once

#ifndef BloodworksControls_h__
#define BloodworksControls_h__

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
	struct KeyData
	{
		MappedKey key;
		std::string keyName;
		Key defaults[4];
	};

	static const std::vector<struct KeyData>& getKeyData()
	{
		return keys;
	}
private:
	static std::vector<struct KeyData> keys;
};

#endif // BloodworksControls_h__