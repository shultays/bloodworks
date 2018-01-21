#pragma once

class Bloodworks;
class LuaBuffController;

class BloodworksLuaWorld
{
	Bloodworks *bloodworks;
	LuaBuffController *buffController;
public:
	BloodworksLuaWorld(Bloodworks *bloodworks);
	~BloodworksLuaWorld();
	void reset();
	void clear();
	void tick();
private:
	void init2();
};