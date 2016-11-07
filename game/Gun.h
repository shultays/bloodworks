#pragma once

#include <string>
#include "cMat.h"
#include "cVec.h"
#include "cResources.h"
#include "sol.h"

class Bloodworks;

class Gun
{
	Bloodworks *bloodworks;
	std::string name;
	std::string artFolder;
	std::string bulletTexturePath;
	cTextureShr bulletTexture;
	Vec2 bulletSize;
	float bulletRadius;

	static int nextGunId;

	std::string scriptName;
	sol::table scriptTable;
	int gunId;

	sol::table luaGun;

	void addBullet();

	float spreadAngle;
	float crosshairDistance;
public:
	Gun() {}
	~Gun();
	void init(Bloodworks *bloodworks, const char *gunData);
	void stop();
	void start();
	void tick(float dt);

	float getMaxCrosshairDistance();
	float getSpreadAngle();
};