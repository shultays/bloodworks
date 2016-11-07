#include "Bloodworks.h"
#include "cRenderable.h"
#include "Player.h"
#include "Bullet.h"
#include "Monster.h"
#include "Gun.h"

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
	gun = new Gun();
	gun->init(this, "resources/basicgun/data.json");
	player->setGun(gun);
	monsterController.init(this);
	bulletController.init(this);

	missionController.init(this);
	missionController.loadMissionController("resources/missions/survival/data.json");
}

Bloodworks::~Bloodworks()
{
	player->setGun(nullptr);
	SAFE_DELETE(bg);
	SAFE_DELETE(player);
	SAFE_DELETE(gun);
	monsterController.clear();
	bulletController.clear();
	missionController.clear();
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
