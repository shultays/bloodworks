#pragma once

#include <vector>

class Bloodworks;
class Bullet;

class BulletController
{
	Bloodworks *bloodworks;
	std::vector<Bullet*> bullets;
public:
	BulletController() {}
	void init(Bloodworks *bloodworks);
	~BulletController();
	void addBullet(Bullet* bullet);
	void tick(float dt);
};