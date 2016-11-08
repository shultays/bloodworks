#include "Bloodworks.h"
#include "cRenderable.h"
#include "Player.h"
#include "Bullet.h"
#include "Monster.h"
#include "Gun.h"
#include "BloodRenderable.h"
#include "cFont.h"

#include <sstream>


void Bloodworks::init()
{
	lua["mission"] = lua.create_table();
	lua["guns"] = lua.create_table();
	lua["bullets"] = lua.create_table();
	lua["monsters"] = lua.create_table();


	bg = new cTexturedQuadRenderable(this, "resources/bg.png", "resources/default");
	bg->setSize(512, 512);
	addRenderable(bg, 0);

	input.hideMouse();

	lastSetTickTime = lastSetRenderTime = 0.0f;
	tickCount = renderCount = 0;

	player = new Player(this);
	Gun *gun = new Gun();
	gun->init(this, "resources/basicgun/data.json");
	guns.push_back(gun);
	player->setGun(gun);

	gun = new Gun();
	gun->init(this, "resources/machinegun/data.json");
	guns.push_back(gun);

	monsterController.init(this);
	bulletController.init(this);

	missionController.init(this);
	missionController.loadMissionController("resources/missions/survival/data.json");

	bloodRenderable = new BloodRenderable(this);
	bloodRenderable->init();
	addRenderable(bloodRenderable, 1);
}

Bloodworks::~Bloodworks()
{
	player->setGun(nullptr);
	SAFE_DELETE(bloodRenderable);
	SAFE_DELETE(bg);
	SAFE_DELETE(player);
	for (auto& gun : guns)
	{
		SAFE_DELETE(gun);
	}
	guns.clear();

	for (auto& drop : drops)
	{
		SAFE_DELETE(drop.renderable);
	}
	drops.clear();
	monsterController.clear();
	bulletController.clear();
	missionController.clear();
}


BloodRenderable* Bloodworks::getBloodRenderable()
{
	return bloodRenderable;
}

void Bloodworks::createGun(const Vec2& pos)
{
	Drop drop;

	do 
	{
		drop.gun = guns[randInt((int)guns.size())];
	} while (drop.gun == player->getGun());
	drop.pos = pos;
	drop.renderable = new cTextRenderable(this, resources.getFont("resources/fontSmallData.txt"), drop.gun->getName(), 11);
	drop.renderable->setPosition(pos - Vec2(drop.gun->getName().size() * 5.0f, 5.0f));
	addRenderable(drop.renderable, 400);

	drops.push_back(drop);
}

void Bloodworks::addDrop(const Vec2& position)
{
	if (drops.size() == 0)
	{
		createGun(position);
	}
}

void Bloodworks::tick(float dt)
{
	lua["dt"] = dt;
	lua["time"] = timer.getTime();

	missionController.tick(dt);

	player->tick(dt);

	monsterController.tick(dt);
	bulletController.tick(dt);

	tickCount++;
	if (timer.getTime() - lastSetTickTime > 1.0f)
	{
		lastSetTickTime += 1.0f;
		std::stringstream ss;
		ss << "FPS " << tickCount;
		debugRenderer.addScreenText(0, ss.str(), 5.0f, 5.0f, FLT_MAX);

		tickCount = 0;
	}


	for(int i=0; i< drops.size(); i++)
	{
		auto& drop = drops[i];
		if (drop.pos.distanceSquared(player->getPos()) < 20.0f * 20.0f)
		{
			player->setGun(drop.gun);
			SAFE_DELETE(drop.renderable);
			drops[i] = drops[(int)drops.size() - 1];
			drops.resize((int)drops.size() - 1);
			i--;
		}
	}

	/*
	if (input.isKeyPressed(mouse_button_right))
	{
		bloodRenderable->addBlood(player->getPos() + Vec2(0.0f, 100.0f));
	}
	*/
}

void Bloodworks::render()
{
	renderCount++;
	if (timer.getTime() - lastSetRenderTime > 1.0f)
	{
		lastSetRenderTime += 1.0f;
		std::stringstream ss;
		ss << "render " << renderCount;
		debugRenderer.addScreenText(1, ss.str(), 5.0f, 35.0f, FLT_MAX);

		renderCount = 0;
	}
}
