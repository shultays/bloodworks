#include "DropController.h"
#include "Bloodworks.h"
#include "cRenderable.h"
#include "cRenderableContainer.h"
#include "cTextRenderable.h"
#include "cTexturedQuadRenderable.h"
#include "cFont.h"
#include "cTexture.h"
#include "Player.h"
#include "Bonus.h"
#include "Monster.h"
#include "Gun.h"
#include "BloodworksControls.h"

DropController::~DropController()
{
	reset();
}

void DropController::spawnGun(const Vec2& position, int forceIndex)
{
	lastDropSpawn = timer.getTime();
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

	cRenderableContainer *group = new cRenderableContainer(bloodworks);

	cTexturedQuadRenderable *renderable = new cTexturedQuadRenderable(bloodworks, drop.gun->getIconPath(), "resources/default");
	Vec2 textureSize = renderable->getTexture()->getDimensions().toVec();
	if (textureSize.w > 32.0f)
	{
		textureSize *= 32.0f / textureSize.w;
	}
	if (textureSize.h > 17.0f)
	{
		textureSize *= 17.0f / textureSize.h;
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
	bloodworks->addRenderable(drop.renderable, PLAYER - 1);

	drops.push_back(drop);
}

void DropController::spawnBonus(const Vec2& position, int forceIndex)
{
	lastDropSpawn = timer.getTime();
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

	cRenderableContainer *group = new cRenderableContainer(bloodworks);

	cTexturedQuadRenderable *renderable = new cTexturedQuadRenderable(bloodworks, drop.bonus->getIconPath(), "resources/default");
	Vec2 textureSize = renderable->getTexture()->getDimensions().toVec() * 0.12f;

	renderable->setWorldMatrix(Mat3::scaleMatrix(textureSize));
	group->addRenderable(renderable);

	cTextRenderable *text = new cTextRenderable(bloodworks, resources.getFont("resources/fontSmallData.txt"), drop.bonus->getName(), 11);
	text->setTextAllignment(TextAlignment::center);
	text->setWorldMatrix(Mat3::translationMatrix(Vec2(0.0f, 15.0f)));
	group->addRenderable(text);

	group->setWorldMatrix(Mat3::translationMatrix(position));
	drop.renderable = group;
	drop.text = text;
	bloodworks->addRenderable(drop.renderable, PLAYER - 1);

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

		drop.text->setVisible(drop.pos.distanceSquared(crosshairPos) < 30.0f * 30.0f || mapper.isKeyDown(GameKey::ShowHints));

		if (drop.pos.distanceSquared(playerPos) < 30.0f * 30.0f)
		{
			if (drop.gun)
			{
				bloodworks->getPlayer()->setGun(drop.gun);
				bloodworks->onPlayerPickedGun(drop.gun);
			}
			else
			{
				drop.bonus->spawnAt(drop.pos);
				bloodworks->onPlayerPickedBonus(drop.bonus, drop.pos);
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

void DropController::spawnDrop(const Vec2& position)
{
	std::vector<float> bonusChances;
	bonusChances.resize(bloodworks->getBonuses().size());
	std::vector<float> gunChances;
	gunChances.resize(bloodworks->getGuns().size());
	float totalChance = 0.0f;
	for (int i = 0; i < bloodworks->getBonuses().size(); i++)
	{
		bonusChances[i] = bloodworks->getBonuses()[i]->getSpawnChance();
		totalChance += bonusChances[i];
	}
	float bonusChance = totalChance;
	for (int i = 0; i < bloodworks->getGuns().size(); i++)
	{
		gunChances[i] = bloodworks->getGuns()[i]->getSpawnChance();
		totalChance += gunChances[i];
	}
	float r = randFloat() * totalChance;

	if (r < bonusChance)
	{
		for (int i = 0; i < bloodworks->getBonuses().size(); i++)
		{
			r -= bonusChances[i];
			if (r < 0.00001f)
			{
				spawnBonus(position, i);
				return;
			}
		}
		spawnBonus(position, (int)bloodworks->getBonuses().size() - 1);
	}
	else
	{
		r -= bonusChance;

		for (int i = 0; i < bloodworks->getGuns().size(); i++)
		{
			r -= gunChances[i];
			if (r < 0.00001f)
			{
				spawnGun(position, i);
				return;
			}
		}
		spawnGun(position, (int)bloodworks->getGuns().size() - 1);
	}
}

void DropController::reset()
{
	for (auto& drop : drops)
	{
		SAFE_DELETE(drop.renderable);
	}
	drops.clear();
	lastDropSpawn = timer.getTime();
}

void DropController::onMonsterDied(Monster* monster, float dropChance)
{
	Vec2 screenMin = bloodworks->getCameraPos() - bloodworks->getScreenDimensions().toVec() * 0.5f * bloodworks->getCameraZoom() + 50;
	Vec2 screenMax = bloodworks->getCameraPos() + bloodworks->getScreenDimensions().toVec() * 0.5f * bloodworks->getCameraZoom() - 50;

	const Vec2& pos = monster->getPosition();

	//if (pos.x > screenMin.x && pos.y > screenMin.y && pos.x < screenMax.x && pos.y < screenMax.y)
	{
		float timeSinceLastDrop = timer.getTime() - lastDropSpawn;

		if (timeSinceLastDrop < 5.0f)
		{
			return;
		}

		float extraDropChance = (timeSinceLastDrop - 10.0f) / 70.0f;
		float r = randFloat();
		if (r < dropChance + extraDropChance)
		{
			spawnDrop(pos);
		}
	}
}

float DropController::getLastSpawnTime() const
{
	return lastDropSpawn;
}

