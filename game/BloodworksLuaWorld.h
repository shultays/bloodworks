#pragma once

class Bloodworks;
class LuaBuffController;

class BloodworksLuaWorld
{
	Bloodworks *bloodworks;
	LuaBuffController *buffController;
public:
	BloodworksLuaWorld(Bloodworks *bloodworks);
	
	void reset();
	void clear();
	void tick();
};