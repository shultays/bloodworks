#include "Bloodworks.h"
#include "cRenderable.h"
#include "Player.h"
#include "Bullet.h"
#include "Monster.h"
#include "Gun.h"

#include <sstream>


void Bloodworks::init()
{
	bg = new cTexturedQuadRenderable(this, "resources/bg.png", "resources/default");
	bg->setSize(512, 512);
	addRenderable(bg, 0);

	input.hideMouse();

	lastSetTickTime = lastSetRenderTime = 0.0f;
	tickCount = renderCount = 0;

	lua["monsters"] = lua.create_table();
	lua["guns"] = lua.create_table();

	player = new Player(this);
	monsterController.init(this);
	gun = new Gun();
	gun->init(this, "resources/basicgun/data.json");
	player->setGun(gun);
}

Bloodworks::~Bloodworks()
{
	player->setGun(nullptr);
	SAFE_DELETE(bg);
	SAFE_DELETE(player);
	SAFE_DELETE(gun);
	monsterController.clear();
}


void Bloodworks::tick(float dt)
{
	lua["dt"] = dt;
	lua["time"] = timer.getTime();

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
