#include "Bloodworks.h"
#include "cRenderable.h"
#include "Player.h"
#include "Bullet.h"
#include "Monster.h"

#include <sstream>


void Bloodworks::init()
{
	bg = new cTexturedQuadRenderable(this, "resources/bg.png", "resources/default");
	bg->setSize(512, 512);
	addRenderable(bg);

	input.hideMouse();

	lastSetTickTime = lastSetRenderTime = 0.0f;
	tickCount = renderCount = 0;

	player = new Player(this);
	monsterController.init(this);
}

Bloodworks::~Bloodworks()
{
	SAFE_DELETE(bg);
	SAFE_DELETE(player);
	for (int i = 0; i < bullets.size(); i++)
	{
		SAFE_DELETE(bullets[i]);
	}
	monsterController.clear();
}

void Bloodworks::addBullet(Bullet *bullet)
{
	bullets.push_back(bullet);
}

void Bloodworks::tick(float dt)
{
	player->tick(dt);

	monsterController.tick(dt);


	for (int i=0; i<bullets.size(); i++)
	{
		bullets[i]->tick(dt);
		if (bullets[i]->isDead)
		{
			SAFE_DELETE(bullets[i]);
			bullets[i] = bullets[bullets.size() - 1];
			bullets.resize(bullets.size() - 1);
			i--;
		}
	}

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
