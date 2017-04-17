#include "KeyMapper.h"

#include "cInput.h"
#include "cGlobals.h"

void KeyMapper::setSavePath(const std::string& file)
{
	for (auto& data : mappedKeys)
	{
		persistent.setDataIfNotExist(data.name, input.getKeyName(data.key));
	}
	persistent.setFileBackup(file);
	for (int i = 0; i < persistent.size(); i++)
	{
		const std::string name = persistent.getName(i);
		const std::string value = persistent.getString(i);
		Key key = input.getKey(value);
		MappedKey index = getKeyMap(name);
		if (index == -1)
		{
			addKeyMap(name, key);
		}
		else
		{
			mappedKeys[i].key = key;
		}
	}
}

MappedKey KeyMapper::addKeyMap(const std::string& name, Key defaultKey)
{
	int addedIndex = getKeyMap(name);
	if (addedIndex >= 0)
	{
		return addedIndex;
	}
	struct KeyInfo keyInfo;
	keyInfo.name = name;
	keyInfo.key = defaultKey;
	mappedKeys.push_back(keyInfo);
	persistent.setDataIfNotExist(name, input.getKeyName(defaultKey));
	persistent.set(name, input.getKeyName(defaultKey));
	mappedIndices[name] = (int)mappedKeys.size() - 1;
	return (int)mappedKeys.size() - 1;
}

MappedKey KeyMapper::getKeyMap(const std::string& name)
{
	auto item = mappedIndices.find(name);
	return item == mappedIndices.end() ? -1 : item->second;
}

bool KeyMapper::isKeyDown(MappedKey key)
{
	return input.isKeyDown(mappedKeys[key].key);
}

bool KeyMapper::isKeyUp(MappedKey key)
{
	return input.isKeyUp(mappedKeys[key].key);
}

bool KeyMapper::isKeyPressed(MappedKey key)
{
	return input.isKeyPressed(mappedKeys[key].key);
}

bool KeyMapper::isKeyReleased(MappedKey key)
{
	return input.isKeyReleased(mappedKeys[key].key);
}

float KeyMapper::getKeyTime(MappedKey key)
{
	return input.getKeyTime(mappedKeys[key].key);
}

void KeyMapper::clearKeyPress(MappedKey key)
{
	input.clearKeyPress(mappedKeys[key].key);
}

void KeyMapper::clearKeyRelease(int key)
{
	input.clearKeyRelease(mappedKeys[key].key);
}
