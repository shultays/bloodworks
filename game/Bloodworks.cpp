#include "Bloodworks.h"
#include "cRenderable.h"
#include "Player.h"
#include "Monster.h"

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
}

void Bloodworks::tick(float dt)
{
	player->tick(dt);
	q->setPosition(player->getPos());
}

