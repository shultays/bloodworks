#pragma once

#ifndef KeyMapper_h__
#define KeyMapper_h__

#include <vector>
#include "cPersistent.h"

typedef int MappedKey;
enum Key;
class KeyMapper
{
	struct KeyInfo
	{
		std::string name;
		Key key;
	};

	std::unordered_map<std::string, int> mappedIndices;
	std::vector<struct KeyInfo> mappedKeys;
	cPersistent persistent;
public:
	KeyMapper()
	{
	}

	void setSavePath(const std::string& file);

	void check()
	{
		persistent.check();
	}

	MappedKey addKeyMap(const std::string& name, Key defaultKey);
	MappedKey getKeyMap(const std::string& name);
	bool isKeyDown(MappedKey key);
	bool isKeyUp(MappedKey key);
	bool isKeyPressed(MappedKey key);
	bool isKeyReleased(MappedKey key);
	float getKeyTime(MappedKey key);
	void clearKeyPress(MappedKey key);
	void clearKeyRelease(int key);
};

#endif // KeyMapper_h__