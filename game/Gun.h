#pragma once

#include <string>
#include "cMat.h"
#include "cVec.h"
#include "cResources.h"
#include "sol.h"

class Bloodworks;
class Bullet;

class Gun
{
	friend class BloodworksLuaWorld;

	Bloodworks *bloodworks;
	std::string name;
	std::string artFolder;
	std::string bulletTexturePath;
	std::string iconPath;
	cTextureShr bulletTexture;
	Vec2 bulletSize;
	Vec2 bulletMeshShift;
	float bulletRadius;
	float bulletSpeed;

	std::string scriptName;
	sol::table scriptTable;
	int id;

	sol::table data;

	Bullet* addBullet();

	float spreadAngle;
	float crosshairDistance;
	IntVec2 damage;

	bool hideSpread;

	bool isTriggered;

public:
	Gun() {}
	~Gun();
	void init(Bloodworks *bloodworks, const char *gunData);
	void stop();
	void start();
	void tick(float dt);

	float getMaxCrosshairDistance();
	float getSpreadAngle();
	int getId();
	sol::table& getScriptTable();
	const std::string& getName()
	{
		return name;
	}
	const std::string& getIconPath() const;
	bool spreadVisible() const;
	void setTriggered(bool param1);
};