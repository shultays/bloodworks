#pragma once

#include "cGlobals.h"
#include "BuffFloat.h"

class LuaBuffController
{
	std::unordered_map<int, BuffFloat*> buffs;
	int nextId;
public:
	LuaBuffController()
	{
		nextId = 0;
	}

	void removeBuff(BuffFloat* buff)
	{
		buffs.erase(buff->buffControllerId);
	}

	void tick()
	{
		for (auto& buff : buffs)
		{
			buff.second->tick();
		}
	}

	void addBuff(BuffFloat* buff)
	{
		buffs[nextId] = buff;
		buff->buffControllerId = nextId;
		nextId++;
		if (nextId > 10000000)
		{
			nextId = 0;
		}
	}
};