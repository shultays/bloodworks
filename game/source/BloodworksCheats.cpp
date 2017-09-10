#include "BloodworksCheats.h"
#include "cGlobals.h"
#include "Bloodworks.h"
#include "MonsterController.h"
#include "cFont.h"
#include "Coral.h"
#include "BloodworksConfig.h"
#include "cSoundManager.h"
#include "Perk.h"
#include "Bonus.h"
#include "Monster.h"
#include "Player.h"
#include "DropController.h"
#include "Bloodrenderable.h"
#include "BulletController.h"
#include "Gun.h"
#include "MissionController.h"
#include "CollisionController.h"
#include "cPostProcess.h"
#include <sstream>

#ifdef HAS_BLOODWORKS_CHEATS

BloodworksCheats* BloodworksCheats::instance = nullptr;

BloodworksCheats::BloodworksCheats(Bloodworks *bloodworks)
{
	this->bloodworks = bloodworks;
	instance = this;
	tickCount = 0;

	lastSetTickTime = lastSetRenderTime = 0.0f;
	tickCount = renderCount = 0;
	moveMonsters = true;
	slowdownBuff = bloodworks->getGlobalUniqueId();

	hasCheats = bloodworks->getConfig()->getBool("cheats", Coral::isDebuggerPresent());

#ifdef DEBUG
	showFPS = true;
#else
	showFPS = Coral::isDebuggerPresent();
#endif
}

BloodworksCheats::~BloodworksCheats()
{
}

bool BloodworksCheats::cheatsEnabled()
{
	return hasCheats;
}

void BloodworksCheats::onTick()
{
	if (input.isKeyDown(key_p) && input.isKeyDown(key_q) && input.isKeyDown(key_space))
	{
		out << "Intentional crash\n";
		int *a = nullptr;
		*a = 42;
	}

	if (showFPS)
	{
		tickCount++;
		if (timer.getRealTime() - lastSetTickTime > 1.0f)
		{
			lastSetTickTime += 1.0f;
			std::stringstream ss;
			ss << "FPS " << tickCount << " Monster " << bloodworks->getMonsterController()->getMonsterCount();
			debugRenderer.addText(0, ss.str(), 5.0f, -24.0f, FLT_MAX, 0xFFFFFFFF, 24.0f, TextAlignment::left, RenderableAlignment::topLeft);

			tickCount = 0;
		}
	}

	if (hasCheats == false)
	{
		return;
	}
	if (bloodworks->isMissionLoaded() == false)
	{
		return;
	}

	const cVector<Perk*>& perks = bloodworks->getPerks();
	const cVector<Bonus*>& bonuses = bloodworks->getBonuses();
	const cVector<Gun*>& guns = bloodworks->getGuns();
	Player *player = bloodworks->getPlayer();
	DropController *dropController = bloodworks->getDropController();
	MonsterController *monsterController = bloodworks->getMonsterController();
	BulletController *bulletController = bloodworks->getBulletController();
	MissionController *missionController = bloodworks->getMissionController();

	if (input.isKeyPressed(key_n))
	{
		showFPS = !showFPS;
		if (showFPS == false)
		{
			debugRenderer.removeText(0);
			debugRenderer.removeText(1);
		}
	}
	if (input.isKeyDown(key_num_minus) || input.isKeyDown(key_num_plus))
	{
		float globalVolume = coral.getSoundManager()->getGlobalVolume();
		if (input.isKeyDown(key_num_plus))
		{
			globalVolume += timer.getNonSlowedDt();
		}
		else
		{
			globalVolume -= timer.getNonSlowedDt();
		}
		clamp(globalVolume, 0.0f, 1.0f);
		coral.getSoundManager()->setGlobalVolume(globalVolume);
		bloodworks->getConfig()->setVolume(globalVolume);
	}

	if (input.isKeyPressed(key_1))
	{
		moveMonsters = !moveMonsters;
	}

	if (input.isKeyPressed(key_2))
	{
		if (input.isKeyDown(key_space))
		{
			for (int i = 0; i < perks.size(); i++)
			{
				while (perks[i]->isTakenFully() == false && perks[i]->getMaxLevel() > 0)
				{
					perks[i]->takeLevel();
					bloodworks->onPerkUsed(perks[i]);
				}
			}
		}
		else
		{
			for (int i = 0; i < perks.size(); i++)
			{
				if (perks[i]->isTakenFully() == false && perks[i]->getScriptName() == "RandomWeapon")
				{
					perks[i]->takeLevel();
					bloodworks->onPerkUsed(perks[i]);
					break;
				}
			}
		}
	}

	if (input.isKeyPressed(key_3))
	{
		std::string s = "Explosion";

		for (int i = 0; i < bonuses.size(); i++)
		{
			if (bonuses[i]->getScriptName() == s)
			{
				bonuses[i]->spawnAt(player->getPosition());
				break;
			}
		}
	}

	if (input.isKeyPressed(key_4))
	{
		for (int i = 0; i < guns.size(); i++)
		{
			dropController->spawnGun(player->getPosition() + Vec2(-100, i * 50.0f - guns.size() * 25.0f), i);
		}

		for (int i = 0; i < bonuses.size(); i++)
		{
			dropController->spawnBonus(player->getPosition() + Vec2(100, i * 50.0f - bonuses.size() * 25.0f), i);
		}
	}

	if (input.isKeyPressed(key_5) && bloodworks->isLevelUpPopupVisible() == false)
	{
		player->doLevelup();
	}

	if (input.isKeyPressed(key_6))
	{
		sol::table t = lua.create_table();
		t["notNearPlayer"] = true;
		for (int i = 0; i < 100; i++)
		{
			Vec2 v;
			do
			{
				v = bloodworks->getMapLimits().getRandomPos();
			} while (v.distanceSquared(bloodworks->getPlayer()->getPosition()) < 20.0f);
			dropController->spawnDrop(v);
		}
	}

	if (input.isKeyPressed(key_v))
	{
		dropController->clearButHighlighted();
	}

	if (input.isKeyPressed(key_7))
	{
		Vec2 dir = Vec2::fromAngle(randFloat(pi_2));
		float damage = randFloat(10);
		bloodworks->getBloodRenderable()->addBlood(player->getPosition(), dir * clamped(damage * 2.7f, 0.0f, 20.0f), 10.0f + randFloat(10.0f));

	}
	if (input.isKeyPressed(key_8) || input.isKeyPressed(key_9))
	{
		Vec2 dir = Vec2::fromAngle(randFloat(pi_2));
		dir = dir * randFloat(5.0f, 20.0f);

		for (int i=0; i<monsterController->getMonsterCount(); i++)
		{
			Monster *m = monsterController->getMonsterAtIndex(i);
			if (m->getDebug())
			{
				input.isKeyPressed(key_8) ? m->spawnBits(dir, 3) : m->spawnBodyParts(dir);
			}
		}
	}

	if (input.isKeyPressed(key_0))
	{
		bloodworks->getBloodRenderable()->reset();
	}

	if (input.isKeyPressed(key_z))
	{
		bloodworks->getMissionController()->getGameSpeedMultiplierBuff().addBuff(slowdownBuff, 0.1f, BuffFloat::multiply_buff);
	}
	if (input.isKeyPressed(key_x))
	{
		bloodworks->getMissionController()->getGameSpeedMultiplierBuff().removeBuff(slowdownBuff);
	}

	{
		static bool zommed = false;
		static float z = 1.0f;
		if (input.isKeyPressed(key_b))
		{
			z = bloodworks->getCameraZoom();
			zommed = !zommed;
			if (zommed == false)
			{
				bloodworks->updateZoom();
			}
		}

		if (input.isKeyDown(key_b) && zommed)
		{
			z += timer.getDt();
			if (z > 4.0f)
			{
				z = 4.0f;
			}
			bloodworks->setCameraZoom(z);
		}
	}

	if (input.isKeyDown(key_f1))
	{
		bulletController->drawDebug();
	}
	if (input.isKeyDown(key_f2))
	{
		monsterController->drawDebug();
	}
	if (input.isKeyDown(key_f3))
	{
		Circle c(player->getPosition(), player->getCollisionRadius());
		c.drawDebug(0xFFFF0000);
		bloodworks->getCollisionController()->drawDebug(true);
	}

	if (input.isKeyPressed(key_g))
	{
		if (player->getMaxHitpoints() < 100000000)
		{
			player->setMaxHitpoints(100000000);
			player->setHitpoints(100000000);
		}
		else
		{
			player->setMaxHitpoints(100);
			player->setHitpoints(100);
		}
	}

	if (input.isKeyPressed(key_q))
	{
		player->killSelf();
	}

	static int z = 0;
	if (input.isKeyPressed(key_c))
	{
		z = (z + 1) % 4;

		for (auto& gun : guns)
		{
			if (gun->getScriptName() == "BananaBomb" && z == 0)
			{
				player->setSecondaryGun(gun);
			}
			if (gun->getScriptName() == "RocketBarrage" && z == 1)
			{
				player->setSecondaryGun(gun);
			}
			if (gun->getScriptName() == "BigLaser" && z == 2)
			{
				player->setSecondaryGun(gun);
			}
			if (gun->getScriptName() == "BlackHole" && z == 3)
			{
				player->setSecondaryGun(gun);
			}
		}
	}
	missionController->onDebugTick();
}

void BloodworksCheats::onMonsterTick(Monster *monster)
{
	if (hasCheats == false)
	{
		return;
	}
	if (moveMonsters == false)
	{
		monster->setPosition(monster->getPreviousPosition());
	}

	if (monster->getDebug())
	{
		if (input.isKeyDown(key_f2) || moveMonsters == false || monster->getDebug() != -1)
		{
			debugRenderer.addCircle(monster->getPosition(), monster->getRadius(), 0.0f, monster->getDebug() == -1 ? 0xFFFFFF00 : 0xFF00FFFF);
		}


		if (moveMonsters == false)
		{
			if (monster->data["posToMove"])
			{
				Vec2 v = monster->data["posToMove"];
				debugRenderer.addCircle(v, 10.0f);
			}
		}

		if (monster->getDebug() == -1)
		{
			if (input.isKeyDown(key_space) && input.isKeyPressed(key_f1))
			{
				out << "----------------------------\n";
				out << "dumping data for monster" << monster->id << "\n";
				lua["dumpTable"](monster->data);
				out << "----------------------------\n";
			}
		}
	}
}

void BloodworksCheats::onRender()
{
	if (showFPS)
	{
		renderCount++;
		if (timer.getRealTime() - lastSetRenderTime > 1.0f)
		{
			lastSetRenderTime += 1.0f;
			std::stringstream ss;
			ss << "Render " << renderCount;
			debugRenderer.addText(1, ss.str(), 5.0f, -24.0f * 2.0f, FLT_MAX, 0xFFFFFFFF, 24.0f, TextAlignment::left, RenderableAlignment::topLeft);

			renderCount = 0;
		}
	}
	if (bloodworks->isMissionLoaded() == false)
	{
		return;
	}
	if (hasCheats == false)
	{
		return;
	}
}

void BloodworksCheats::onLoadMission()
{
	if (hasCheats == false)
	{
		return;
	}
	const cVector<Gun*>& guns = bloodworks->getGuns();
	Player *player = bloodworks->getPlayer();

	for (auto& gun : guns)
	{
		if (gun->getScriptName() == "Uzi")
		{
			//player->setGun(gun);
		}
		if (gun->getScriptName() == "BananaBomb")
		{
			//player->setSecondaryGun(gun);
		}
		if (gun->getScriptName() == "RocketBarrage")
		{
			//player->setSecondaryGun(gun);
		}
		if (gun->getScriptName() == "BigLaser")
		{
			//player->setSecondaryGun(gun);
		}
		if (gun->getScriptName() == "BlackHole")
		{
			//player->setSecondaryGun(gun);
		}
	}
}

void BloodworksCheats::onMonsterPreTick(Monster* monster)
{
	if (hasCheats == false)
	{
		return;
	}
	if (monster->getDebug())
	{
		if (input.isKeyPressed(key_f5))
		{
			monster->setDebug(1);
		}
		if (input.isKeyPressed(key_f6))
		{
			monster->setDebug(0);
		}
		if (input.isKeyPressed(key_f7))
		{
			doBreak();
		}

	}
}

void BloodworksCheats::onInit()
{
	static std::string testGame = "";
	if (Coral::isDebuggerPresent())
	{
		coral.setFullScreen(false);
	}
	if (Coral::isDebuggerPresent() && testGame.size())
	{
		coral.getSoundManager()->setGlobalVolume(0.0f);
		bloodworks->loadMission(testGame);
	}
	testGame = "";
	if (hasCheats == false)
	{
		return;
	}
}

#endif