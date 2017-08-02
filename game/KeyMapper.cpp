#include "KeyMapper.h"

#include "cInput.h"
#include "cGlobals.h"

void KeyMapper::setSavePath(const std::string& file)
{
	for (auto& data : mappedKeys)
	{
		cVector<std::string> keyNames;
		for (int i = 0; i < data.num; i++)
		{
			keyNames.push_back(input.getKeyName(data.keys[i]));
		}
		persistent.setDataIfNotExist(data.name, keyNames);
	}
	persistent.setFileBackup(file);
	for (int i = 0; i < persistent.size(); i++)
	{
		const std::string name = persistent.getName(i);
		cVector<std::string>& values = persistent.getStringList(i);
		Key keys[4] = { key_invalid, key_invalid, key_invalid, key_invalid };
		for (int j = 0; j < values.size(); j++)
		{
			keys[j] = input.getKey(values[j]);
		}

		MappedKey index = getKeyMap(name);
		if (index == -1)
		{
			addKeyMap(name, keys[0], keys[1], keys[2], keys[3]);
		}
		else
		{
			mappedKeys[index].num = values.size();
			for (int j = 0; j < values.size(); j++)
			{
				mappedKeys[index].keys[j] = keys[j];
			}
		}
	}
}

MappedKey KeyMapper::addKeyMap(const std::string& name, Key defaultKey0, Key defaultKey1, Key defaultKey2, Key defaultKey3)
{
	int addedIndex = getKeyMap(name);
	if (addedIndex >= 0)
	{
		return addedIndex;
	}
	struct KeyInfo keyInfo;
	keyInfo.name = name;
	keyInfo.keys[0] = defaultKey0;
	keyInfo.keys[1] = defaultKey1;
	keyInfo.keys[2] = defaultKey2;
	keyInfo.keys[3] = defaultKey3;
	keyInfo.num = (keyInfo.keys[0] > 0) + (keyInfo.keys[1] > 0) + (keyInfo.keys[2] > 0) + (keyInfo.keys[3] > 0);
	cVector<std::string> keyNames;
	for (int i = 0; i < keyInfo.num; i++)
	{
		keyNames.push_back(input.getKeyName(keyInfo.keys[i]));
	}
	mappedKeys.push_back(keyInfo);
	persistent.setDataIfNotExist(name, keyNames);
	persistent.set(name, keyNames);
	mappedIndices[name] = mappedKeys.size() - 1;
	return mappedKeys.size() - 1;
}


MappedKey KeyMapper::getKeyMap(const std::string& name)
{
	auto item = mappedIndices.find(name);
	return item == mappedIndices.end() ? -1 : item->second;
}

void KeyMapper::setKeyMap(MappedKey key, Key defaultKey0, Key defaultKey1 /*= (Key)0*/, Key defaultKey2 /*= (Key)0*/, Key defaultKey3 /*= (Key)0*/)
{
	auto& keyInfo = mappedKeys[key];
	keyInfo.keys[0] = defaultKey0;
	keyInfo.keys[1] = defaultKey1;
	keyInfo.keys[2] = defaultKey2;
	keyInfo.keys[3] = defaultKey3;
	keyInfo.num = (keyInfo.keys[0] > 0) + (keyInfo.keys[1] > 0) + (keyInfo.keys[2] > 0) + (keyInfo.keys[3] > 0);
	cVector<std::string> keyNames;
	for (int i = 0; i < keyInfo.num; i++)
	{
		keyNames.push_back(input.getKeyName(keyInfo.keys[i]));
	}
	persistent.set(keyInfo.name, keyNames);
}

bool KeyMapper::isKeyDown(MappedKey key)
{
	for (int i = 0; i < mappedKeys[key].num; i++)
	{
		if (input.isKeyDown(mappedKeys[key].keys[i])) return true;
	}
	return false;
}

bool KeyMapper::isKeyUp(MappedKey key)
{
	for (int i = 0; i < mappedKeys[key].num; i++)
	{
		if (input.isKeyUp(mappedKeys[key].keys[i]) == false) return false;
	}
	return true;
}

bool KeyMapper::isKeyPressed(MappedKey key)
{
	for (int i = 0; i < mappedKeys[key].num; i++)
	{
		if (input.isKeyPressed(mappedKeys[key].keys[i])) return true;
	}
	return false;
}

bool KeyMapper::isKeyReleased(MappedKey key)
{
	for (int i = 0; i < mappedKeys[key].num; i++)
	{
		if (input.isKeyReleased(mappedKeys[key].keys[i])) return true;
	}
	return false;
}

float KeyMapper::getKeyTime(MappedKey key)
{
	float maxTime = -1.0f;

	for (int i = 0; i < mappedKeys[key].num; i++)
	{
		float t = input.getKeyTime(mappedKeys[key].keys[i]);
		if (maxTime < t)
		{
			maxTime = t;
		}
	}
	return maxTime;
}

void KeyMapper::clearKeyPress(MappedKey key)
{
	for (int i = 0; i < mappedKeys[key].num; i++)
	{
		input.clearKeyPress(mappedKeys[key].keys[i]);
	}
}

void KeyMapper::clearKeyRelease(MappedKey key)
{
	for (int i = 0; i < mappedKeys[key].num; i++)
	{
		input.clearKeyRelease(mappedKeys[key].keys[i]);
	}
}

const Key* KeyMapper::getMappedKeys(MappedKey key) const
{
	return mappedKeys[key].keys;
}
