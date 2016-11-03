#include "Bloodworks.h"
#include "cRenderable.h"
#include "Player.h"
#include "Monster.h"
#include "cFont.h"

#include <sstream>
cTextRenderable *f;
cTexturedQuadRenderable *q;


void Bloodworks::init()
{
	bg = new cTexturedQuadRenderable(this, "resources/bg.png", "resources/default");
	bg->setSize(512, 512);
	addRenderable(bg);

	q = new cTexturedQuadRenderable(this, "resources/s.png", "resources/default");
	q->setSize(1);
	addRenderable(q);


	for (int i = 0; i < 5; i++)
	{
		monsters.push_back(new Monster(this));
	}

	player = new Player(this);
	input.hideMouse();

	//f = new cTextRenderable(this, font, "ASD ASDASD", 30.0f);
	//addRenderable(f);

	lastSetTickTime = lastSetRenderTime = 0.0f;
	tickCount = renderCount = 0;
}

void Bloodworks::tick(float dt)
{
	player->tick(dt);

	for (auto& monster : monsters)
	{
		monster->tick(dt);
	}


	q->setPosition(player->getPos());


	tickCount++;
	if (time.getTime() - lastSetTickTime > 1.0f)
	{
		lastSetTickTime += 1.0f;
		std::stringstream ss;
		ss << "FPS " << tickCount;
		debugRenderer.addTextData(0, ss.str(), 5.0f, 5.0f, FLT_MAX);

		tickCount = 0;
	}
}

void Bloodworks::render()
{
	renderCount++;
	if (time.getTime() - lastSetRenderTime > 1.0f)
	{
		lastSetRenderTime += 1.0f;
		std::stringstream ss;
		ss << "FPS " << renderCount;
		debugRenderer.addTextData(1, ss.str(), 5.0f, 35.0f, FLT_MAX);

		renderCount = 0;
	}
}
