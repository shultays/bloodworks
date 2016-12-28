#include "DropController.h"
#include "Bloodworks.h"
#include "cRenderable.h"
#include "cFont.h"
#include "cTexture.h"
#include "Player.h"
#include "Bonus.h"
#include "Gun.h"

DropController::~DropController()
{
	reset();
}

void DropController::createGun(const Vec2& position, int forceIndex)
{
	Drop drop;
	drop.bonus = nullptr;
	drop.time = timer.getTime();
	drop.canFadeout = false;
	auto& guns = bloodworks->getGuns();

	if (forceIndex == -1)
	{
		int t = 10;
		do
		{
			drop.gun = guns[randInt((int)guns.size())];
		} while (drop.gun == bloodworks->getPlayer()->getGun() && t-- > 0);
	}
	else
	{
		drop.gun = guns[forceIndex];
	}

	drop.pos = position;

	cRenderableGroup *group = new cRenderableGroup(bloodworks);

	cTexturedQuadRenderable *renderable = new cTexturedQuadRenderable(bloodworks, drop.gun->getIconPath(), "resources/default");
	Vec2 textureSize = renderable->getTexture()->getDimensions().toVec();
	if (textureSize.w > 30.0f)
	{
		textureSize *= 30.0f / textureSize.w;
	}
	if (textureSize.h > 15.0f)
	{
		textureSize *= 15.0f / textureSize.h;
	}
	renderable->setWorldMatrix(Mat3::scaleMatrix(textureSize).rotateBy(-pi * 0.15f));
	group->addRenderable(renderable);

	cTextRenderable *text = new cTextRenderable(bloodworks, resources.getFont("resources/fontSmallData.txt"), drop.gun->getName(), 11);
	text->setTextAllignment(TextAlignment::center);
	text->setWorldMatrix(Mat3::translationMatrix(Vec2(0.0f, 15.0f)));
	group->addRenderable(text);

	group->setWorldMatrix(Mat3::translationMatrix(position));
	drop.renderable = group;
	drop.text = text;
	bloodworks->addRenderable(drop.renderable, OBJECT_GUI);

	drops.push_back(drop);
}

void DropController::createBonus(const Vec2& position, int forceIndex)
{
	Drop drop;
	drop.canFadeout = false;
	drop.time = timer.getTime();
	auto& bonuses = bloodworks->getBonuses();
	if (forceIndex >= 0)
	{
		drop.bonus = bonuses[forceIndex];
	}
	else
	{
		drop.bonus = bonuses[randInt((int)bonuses.size())];
	}
	drop.gun = nullptr;
	drop.pos = position;

	cRenderableGroup *group = new cRenderableGroup(bloodworks);

	cTexturedQuadRenderable *renderable = new cTexturedQuadRenderable(bloodworks, drop.bonus->getIconPath(), "resources/default");
	Vec2 textureSize = renderable->getTexture()->getDimensions().toVec() * 0.10f;

	renderable->setWorldMatrix(Mat3::scaleMatrix(textureSize));
	group->addRenderable(renderable);

	cTextRenderable *text = new cTextRenderable(bloodworks, resources.getFont("resources/fontSmallData.txt"), drop.bonus->getName(), 11);
	text->setTextAllignment(TextAlignment::center);
	text->setWorldMatrix(Mat3::translationMatrix(Vec2(0.0f, 15.0f)));
	group->addRenderable(text);

	group->setWorldMatrix(Mat3::translationMatrix(position));
	drop.renderable = group;
	drop.text = text;
	bloodworks->addRenderable(drop.renderable, OBJECT_GUI);

	drops.push_back(drop);
}

void DropController::tick()
{
	Vec2 playerPos = bloodworks->getPlayer()->getPosition();
	Vec2 crosshairPos = playerPos + bloodworks->getPlayer()->getCrosshairPos();
	for (int i = 0; i < drops.size(); i++)
	{
		auto& drop = drops[i];
		bool remove = false;

		if (timer.getTime() - drop.time < 0.3f)
		{
			float s = (timer.getTime() - drop.time) / 0.34f;
			drop.renderable->setColor(Vec4(1.0f, 1.0f, 1.0f, s));
		}
		else
		{
			drop.renderable->setColor(Vec4(1.0f, 1.0f, 1.0f, 1.0f));
		}

		drop.text->setVisible(drop.pos.distanceSquared(crosshairPos) < 40.0f * 40.0f);

		if (drop.pos.distanceSquared(playerPos) < 20.0f * 20.0f)
		{
			if (drop.gun)
			{
				bloodworks->getPlayer()->setGun(drop.gun);
			}
			else
			{
				drop.bonus->spawnAt(drop.pos);
			}
			remove = true;
		}

		if (drop.time + 20.0f < timer.getTime())
		{
			remove = true;
		}
		else if (drop.time + 15.0f < timer.getTime())
		{
			bool isFadeout = ((int)(timer.getTime() * 3)) % 2 != 0;
			if (isFadeout == false)
			{
				drop.canFadeout = true;
			}
			drop.renderable->setColor(Vec4(1.0f, 1.0f, 1.0, (isFadeout && drop.canFadeout) ? 0.2f : 1.0f));
		}

		if (remove)
		{
			SAFE_DELETE(drop.renderable);
			drops[i] = drops[(int)drops.size() - 1];
			drops.resize((int)drops.size() - 1);
			i--;
		}
	}
}

void DropController::addDrop(const Vec2& position)
{
	if (randBool() && input.isKeyDown(key_1) == false)
	{
		createBonus(position);
	}
	else
	{
		createGun(position);
	}
}

void DropController::reset()
{
	for (auto& drop : drops)
	{
		SAFE_DELETE(drop.renderable);
	}
	drops.clear();
}

