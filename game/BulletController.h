#pragma once

#include <vector>
#include <unordered_map>

class Bloodworks;
class Bullet;

class BulletController
{
	Bloodworks *bloodworks;
	std::vector<Bullet*> bullets;
	std::unordered_map<int, Bullet*> bulletMap;
public:
	BulletController() {}
	void init(Bloodworks *bloodworks);
	void clear();
	~BulletController();
	void addBullet(Bullet* bullet);
	void tick(float dt);
};