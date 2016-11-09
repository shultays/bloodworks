#pragma once

#include <vector>
#include <unordered_map>
#include "cGrid.h"
#include "Bullet.h"

class Bloodworks;

class BulletController
{
	Bloodworks *bloodworks;
	std::vector<Bullet*> bullets;
	std::unordered_map<int, Bullet*> bulletMap;
	cGrid<Bullet> grid;
public:
	BulletController() {}
	void init(Bloodworks *bloodworks);
	void clear();
	~BulletController();
	void addBullet(Bullet* bullet);
	void tick(float dt);
};