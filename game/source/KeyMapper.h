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
		//int num;
		Key keys[4];
	};

	std::unordered_map<std::string, int> mappedIndices;
	cVector<struct KeyInfo> mappedKeys;
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

	MappedKey addKeyMap(const std::string& name, Key defaultKey0, Key defaultKey1 = (Key)0, Key defaultKey2 = (Key)0, Key defaultKey3 = (Key)0);
	MappedKey getKeyMap(const std::string& name);
	
	void setKeyMap(MappedKey index, Key defaultKey0, Key defaultKey1 = (Key)0, Key defaultKey2 = (Key)0, Key defaultKey3 = (Key)0);

	bool isKeyDown(MappedKey key);
	bool isKeyUp(MappedKey key);
	bool isKeyPressed(MappedKey key);
	bool isKeyReleased(MappedKey key);
	float getKeyTime(MappedKey key);
	void clearKeyPress(MappedKey key);
	void clearKeyRelease(MappedKey key);


	// TODO dirty, fix those!
	template<class T>
	bool isKeyDown(T key)
	{
		return isKeyDown((MappedKey)key);
	}
	template<class T>
	bool isKeyUp(T key)
	{
		return isKeyUp((MappedKey)key);
	}
	template<class T>
	bool isKeyPressed(T key)
	{
		return isKeyPressed((MappedKey)key);
	}
	template<class T>
	bool isKeyReleased(T key)
	{
		return isKeyReleased((MappedKey)key);
	}
	template<class T>
	float getKeyTime(T key)
	{
		return getKeyTime((MappedKey)key);
	}
	template<class T>
	void clearKeyPress(T key)
	{
		clearKeyPress((MappedKey)key);
	}
	template<class T>
	void clearKeyRelease(T key)
	{
		clearKeyRelease((MappedKey)key);
	}
	const Key* getMappedKeys(MappedKey key) const;
};

#endif // KeyMapper_h__