#include "Monster.h"
#include "Bloodworks.h"
#include "cAnimatedRenderable.h"
#include "cGlobals.h"
#include "cTools.h"
#include "cFont.h"
#include "cTexture.h"
#include "Bloodworks.h"
#include "Player.h"
#include "BloodRenderable.h"
#include "cTexturedQuadRenderable.h"
#include "cTextRenderable.h"
#include "MonsterController.h"
#include "Gun.h"
#include "Bullet.h"
#include "cSound.h"
#include "cParticle.h"
#include "BloodworksControls.h"
#include "MonsterTemplate.h"
#include "BloodworksCheats.h"
#include "cTimeProfiler.h"
#include "CollisionController.h"
#include "cCircle.h"

Monster::Monster(Bloodworks *bloodworks)
{
	this->bloodworks = bloodworks;
	id = bloodworks->getUniqueId();
	lastRunCheck = -1;
	debugVal = 0;
	addIgnoreId(id);
}

void Monster::init(const MonsterTemplate* monsterTemplate)
{
	this->monsterTemplate = monsterTemplate;
	name = monsterTemplate->name;
	experienceMultiplier = 1.0f;
	scoreMultiplier = 1.0f;
	isDead = false;
	removeOnDead = true;
	lastSoundTime = 0.0f;
	pathCheckDistance = 150.0f;
	textureSize = monsterTemplate->size;
	textureShift = monsterTemplate->textureShift;
	hitPoint = monsterTemplate->hitPoint;
	hasBlood = monsterTemplate->hasBlood;
	hasGibs = true;
	knockbackResistance.setBaseValue(1.0f);
	animationSpeed = 1.0f;

	lastHitSoundPlayTime = timer.getTime();
	lastBloodTime = timer.getTime();
	renderable = new cAnimatedTexturedQuadRenderable(bloodworks, "resources/default");
	renderable->addAnimation(monsterTemplate->animationData);
	bloodworks->addRenderable(renderable, MONSTERS);

	setScale(1.0f);

	renderable->setDefaultAnimation(renderable->getAnimationIndex("stand"));

	healthRenderable = new cTextRenderable(bloodworks, resources.getFont("resources/fontSmallData.txt"), "", 10);
	healthRenderable->setTextAlignment(TextAlignment::center);
	healthRenderable->setVisible(false);
	bloodworks->addRenderable(healthRenderable, OBJECT_GUI);

	spriteAngle = -100000.0f;
	moveAngle = randFloat(-pi, pi);
	moveDir = Vec2::fromAngle(moveAngle);
	moveSpeed = 0.0f;

	data = lua.create_table_with();

	scriptTable = monsterTemplate->scriptTable;

	bulletRadius = monsterTemplate->bulletRadius;
	collisionRadius = monsterTemplate->collisionRadius;
	position = 100.0f;
	prevPosition = position;
	scale = 1.0f;
	lastBitTime = timer.getTime();
	canGetOneShooted = true;
	hasCollision = true;

	dropChance = 0.05f;
	firstTick = true;
	luaShouldHit = scriptTable["shouldHit"];
	luaTick = scriptTable["onTick"];
	luaHit = scriptTable["onHit"];
	luaKill = scriptTable["onKilled"];
	scriptTable["init"](this);
}

void Monster::setScale(float scale)
{
	this->scale = scale;
	textureSize = monsterTemplate->size * scale;
	textureShift = monsterTemplate->textureShift * scale;
	bulletRadius = monsterTemplate->bulletRadius * scale;
	collisionRadius = monsterTemplate->collisionRadius * scale;
	Vec2 originalSize = renderable->getTextureSize().toVec();
	partScale = (textureSize / originalSize);
}

Monster::~Monster()
{
	bloodworks->addOrphanParticle(particles);
	particles.clear();
	SAFE_DELETE(renderable);
	SAFE_DELETE(healthRenderable);
}

void Monster::tick()
{

	static cAccumulatedTimeProfiler& p = Coral::createAccumulatedTimeProfile("Monster::tickSum");
	p.start();

#ifdef HAS_BLOODWORKS_CHEATS
	BloodworksCheats::instance->onMonsterPreTick(this);
#endif

	moveSpeedMultiplier.tick();
	knockbackResistance.tick();
	bool colorChanged = colorMultiplier.tick();
	if (colorChanged && renderable)
	{
		renderable->setColor(colorMultiplier.getBuffedValue());
	}
	if (luaTick && timer.getDt() > 0.0f)
	{
		static cAccumulatedTimeProfiler& p = Coral::createAccumulatedTimeProfile("Monster::luaTick");
		p.start();
		luaTick(this);
		p.stop();
	}
	renderable->setSpeedMultiplier(this->animationSpeed);
	moveDir = Vec2::fromAngle(moveAngle);
	//healthRenderable->setVisible(input.isKeyDown(key_space));
	cVector<int> toTrigger;
	for (int i = timers.size() - 1; i >= 0; i--)
	{
		if (timers[i].time < timer.getTime())
		{
			toTrigger.push_back(i);
		}
	}

	for (auto& i : toTrigger)
	{
		Timer& t = timers[i];
		scriptTable[t.func](t.args);
		
		timers[i] = timers[timers.size() - 1]; // todo is this correct?
		timers.resize(timers.size() - 1);
	}
	moveVelocity = Vec2::fromAngle(moveAngle) * moveSpeed * moveSpeedMultiplier.getBuffedValue() * bloodworks->getPlayer()->getGlobalMonsterSpeedMultiplier();
	prevPosition = position;
	if (moveSpeed > 0.0f)
	{
		position += moveVelocity * timer.getDt();
	}

	float dt = timer.getDt();
	for (int i = 0; i < knockbacks.size(); i++)
	{
		Knockback& k = knockbacks[i];
		bool remove = false;
		float pushAmount = dt;
		if (k.duration < -0.5f)
		{
			pushAmount = dt;
		}
		else if (k.duration < dt)
		{
			pushAmount = k.duration;
		}
		k.duration -= dt;
		position += k.speed * pushAmount * knockbackResistance.getBuffedValue();

		if (k.duration <= 0.0f)
		{
			knockbacks.swapToTailRemove(i);
			i--;
		}
	}

	if (isDead)
	{
		position = prevPosition;
	}
	if (hasCollision)
	{
		Vec2 newPosition = position;
		bool solved = false;
		for (int i = 0; i < 5; i++)
		{
			Vec2 solver = bloodworks->getCollisionController()->getLongestSolver(Circle(position, collisionRadius), CollisionController::no_monster_collision);
			if (solver.lengthSquared() < 0.0001f)
			{
				solved = true;
				break;
			}
			else
			{
				position += solver;
			}
		}

		if (solved == false)
		{
			for (int i = 1; i < 10; i++)
			{
				position = (prevPosition - newPosition) * ((float)i) / 9.0f + newPosition;
				if (i == 9)
				{
					position = prevPosition;
				}
				Vec2 solver = bloodworks->getCollisionController()->getLongestSolver(Circle(position, collisionRadius), CollisionController::no_monster_collision);
				if (solver.lengthSquared() == 0.0f)
				{
					break;
				}
			}
		}
	}

	clampPos();
#ifdef HAS_BLOODWORKS_CHEATS
	BloodworksCheats::instance->onMonsterTick(this);
#endif
	if (healthRenderable->isVisible())
	{
		std::stringstream ss;
		ss << (int)hitPoint;

		healthRenderable->setText(ss.str());
		healthRenderable->setWorldMatrix(Mat3::translationMatrix(position + Vec2(0.0f, bulletRadius + 10.0f)));
	}

	Mat3 mat = Mat3::identity();
	mat.scaleBy(textureSize);
	mat.translateBy(textureShift);
	if (spriteAngle > -1000.0f)
	{
		mat.rotateBy(-pi_d2 - spriteAngle);
	}
	else
	{
		mat.rotateBy(-pi_d2 - moveAngle);
	}
	mat.translateBy(position);
	renderable->setWorldMatrix(mat);

	firstTick = false;
	p.stop();
}


void Monster::addTimer(float timeToTrigger, const std::string& func, sol::table args, bool looped /*= false*/)
{
	Timer t;
	t.time = timeToTrigger + timer.getTime();
	t.func = func;
	t.args = args;
	t.looped = looped;
	timers.push_back(t);
}

void Monster::playAnimation(std::string anim, float startPercentage)
{
	renderable->playAnimation(anim, startPercentage);
}

void Monster::doDamage(int damage, const Vec2& dir)
{
	sol::table t = lua.create_table();
	doDamageWithArgs(damage, dir, t);
}

void Monster::doDamageWithArgs(int damage, const Vec2& dirInput, sol::table& args)
{
	if (isDead)
	{
		return;
	}
	Vec2 dir = dirInput;
	if (dir.lengthSquared() < 0.01f)
	{
		dir = -moveDir;
	}
	damage = (int)(damage * bloodworks->getPlayer()->getDamageMultiplier());
	damage = bloodworks->onMonsterDamaged(this, damage, dir, args);

	if (damage <= 0)
	{
		return;
	}
	hitPoint -= damage;

	if (luaHit)
	{
		luaHit(this, damage, dir, args);
	}

	if (hitPoint <= 0)
	{
		if (damage > 0)
		{
			spawnBits(dir * clamped(damage * 0.3f, 0.0f, 20.0f), 3);
			killSelf(dir * clamped(damage * 0.3f, 0.0f, 20.0f));
		}
	}
	else
	{
		if (damage > 0)
		{
			Vec2 spawnDir = Vec2::fromAngle(dir.toAngle() - 0.2f + 0.4f * randFloat());
			float r = randFloat() * 0.3f + 0.4f;
			r = sqrtf(r);
			Vec2 shift = spawnDir * r * collisionRadius * 2.0f;

			if (lastBloodTime + 0.1f < timer.getTime())
			{
				lastBloodTime = timer.getTime();

				if (hasBlood)
				{
					bloodworks->getBloodRenderable()->addBlood(position + shift, dir * clamped(damage * 0.3f, 0.0f, 20.0f), 10.0f);
				}
				spawnBits(dir * clamped(damage * 0.3f, 0.0f, 20.0f));
			}
			if (monsterTemplate->hitSounds.size() && lastHitSoundPlayTime + 0.3f < timer.getTime() && (damage > 15 || randFloat() < 0.3f))
			{
				lastHitSoundPlayTime = timer.getTime();
				cSoundSampleShr s = monsterTemplate->hitSounds[randInt(monsterTemplate->hitSounds.size())];
				bloodworks->playSoundAtMap(position, s, 0.7f);
			}
		}
	}
}

Vec2 Monster::getPathPos(const Vec2& target)
{
	if (hasCollision == false)
	{
		return target;
	}
	Vec2 dir = target - this->position;
	float len = dir.normalize();
	const float radius = collisionRadius * 0.5f;
	float t = bloodworks->getCollisionController()->getRayDistance(position + dir * radius * 0.5f, dir * pathCheckDistance, 0.0f, CollisionController::no_monster_collision);
	//debugRenderer.addLine(position + dir * 10.0f, position + dir * checkDistance);
	if (t > pathCheckDistance - 10.0f)
	{
		return target;
	}
	float angle = dir.toAngle();
	for (int i = 1; i < 8; i++)
	{
		float shift = i * pi_d2/ 5.0f;

		Vec2 dirRotated;
		dirRotated = Vec2::fromAngle(angle + shift);
		t = bloodworks->getCollisionController()->getRayDistance(position + dirRotated * radius * 0.5f, dirRotated * pathCheckDistance, 0.0f, CollisionController::no_monster_collision);
		//debugRenderer.addLine(position, position + dirRotated * checkDistance);
		if (t > pathCheckDistance - 10.0f)
		{
			//debugRenderer.addLine(position, position + dirRotated * checkDistance, 0.0f, 0xFFFF0000);
			return position + dirRotated * pathCheckDistance;
		}

		dirRotated = Vec2::fromAngle(angle - shift);
		t = bloodworks->getCollisionController()->getRayDistance(position + dirRotated * 5.0f, dirRotated * pathCheckDistance, 0.0f, CollisionController::no_monster_collision);
		//debugRenderer.addLine(position, position + dirRotated * checkDistance);
		if (t > pathCheckDistance - 10.0f)
		{
			//debugRenderer.addLine(position, position + dirRotated * checkDistance, 0.0f, 0xFFFF0000);
			return position + dirRotated * pathCheckDistance;
		}
	}
	return target;
}

bool Monster::isVisible() const
{
	return renderable->isVisible();
}

void Monster::setVisible(bool visible)
{
	renderable->setVisible(visible);
}

void Monster::addIgnoreId(int id)
{
	if (ignoreIds.size() == 0 || ignoreIds[ignoreIds.size() - 1] < id)
	{
		ignoreIds.push_back(id);
	}
	else
	{
		ignoreIds.push_back(INT_MAX);
		int i = ignoreIds.size() - 1;
		while (i > 0 && ignoreIds[i - 1] > id)
		{
			ignoreIds[i] = ignoreIds[i - 1];
			i--;
		}
		ignoreIds[i] = id;
	}
}

bool Monster::hasIgnoreId(int id) const
{
	return std::binary_search(ignoreIds.begin(), ignoreIds.end(), id);
}

void Monster::copyIgnoreId(Monster *other)
{
	for (auto id : other->ignoreIds)
	{
		addIgnoreId(id);
	}
}

void Monster::setPosition(const Vec2& pos)
{
	this->position = pos;
	clampPos();
	prevPosition = position;
	bloodworks->getMonsterController()->relocateMonster(this);
}

bool Monster::shouldHit(Bullet *bullet)
{
	if (firstTick)
	{
		return false;
	}
	if (luaShouldHit)
	{
		return luaShouldHit(this, bullet->getGun(), bullet);
	}
	return bloodworks->getMonsterController()->shouldHit(this, bullet->getGun(), bullet);
}

bool Monster::shouldHit(Gun *gun)
{
	if (luaShouldHit)
	{
		return luaShouldHit(this, gun, nullptr);
	}
	return bloodworks->getMonsterController()->shouldHit(this, gun, nullptr);
}

cParticle* Monster::addParticleSpawner(const std::string& name, sol::table& args)
{
	cParticle *particle = new cParticle(bloodworks, bloodworks->getParticleTemplate(name), args);
	particles.push_back(particle);
	bloodworks->addRenderable(particle, renderable->getLevel() + 1);
	return particle;
}


void Monster::spawnParticleInternal(cParticle *particleToSpawn, sol::table* params)
{
	spawnParticleShiftedInternal(Vec2::zero(), particleToSpawn, params);
}

void Monster::spawnParticleShiftedInternal(const Vec2& shift, cParticle *particleToSpawn, sol::table* params)
{
	if (particleToSpawn)
	{
		particleToSpawn->addParticleInternal(position + shift, params);
	}
	else
	{
		for (auto& particle : particles)
		{
			particle->addParticleInternal(position + shift, params);
		}
	}
}


void Monster::spawnParticle(cParticle *particle, sol::table& params)
{
	spawnParticleInternal(particle, &params);
}

void Monster::spawnParticleShifted(const Vec2& shift, cParticle *particle, sol::table& params) // todo remove this
{
	spawnParticleShiftedInternal(shift, particle, &params);
}

void Monster::spawnParticleWithoutArgs(cParticle *particle)
{
	spawnParticleInternal(particle, nullptr);
}

void Monster::spawnParticleShiftedWithoutArgs(const Vec2& shift, cParticle *particle)
{
	spawnParticleShiftedInternal(shift, particle, nullptr);
}

void Monster::addKnockback(const Vec2& speed, float duration)
{
	Knockback& k = knockbacks.insertAndGetReference();
	k.speed = speed;
	k.duration = duration;
}

void Monster::setDrawLevel(int level)
{
	renderable->setLevel(level);
}

void Monster::modifyDrawLevel(int level)
{
	renderable->setLevel(renderable->getLevel() + level);
}

int Monster::getDrawLevel() const
{
	return renderable->getLevel();
}

void Monster::spawnBodyParts(const Vec2& blowDir)
{
	if (blowDir.isNonZero())
	{
		if (hasBlood)
		{
			bloodworks->getBloodRenderable()->addBlood(position, blowDir * (1.0f + randFloat() * 0.5f), 15.0f + randFloat() * 5.0f);
		}

		cVector<int> parts;
		int maxCount = monsterTemplate->bodyParts.size();
		if (maxCount > 0)
		{
			int partCount = randInt(2, maxCount);
			if (partCount > maxCount)
			{
				partCount = maxCount;
			}
			for (int i = 0; i < partCount; i++)
			{
				int t = randInt(maxCount);
				for (int j = 0; j < parts.size(); j++)
				{
					if (parts[j] == t)
					{
						i--;
						t = -1;
						break;
					}
				}
				if (t != -1)
				{
					parts.push_back(t);
				}
			}
		}

		if (hasGibs)
		{
			for (int i : parts)
			{
				cTextureShr s = monsterTemplate->bodyParts[i].texture;
				cTexturedQuadRenderable *partRenderable = new cTexturedQuadRenderable(bloodworks, s->getName(), "resources/default");
				partRenderable->setColor(renderable->getColor());
				Mat3 mat = renderable->getWorldMatrix();
				partRenderable->setWorldMatrix(mat);
				bloodworks->getBloodRenderable()->addBodyPart(partRenderable,
					position + monsterTemplate->bodyParts[i].shift * partScale * Mat2::rotation(moveAngle),
					monsterTemplate->bodyParts[i].texture->getDimensions().toVec() * partScale,
					moveAngle - pi_d2,
					blowDir * 2.0f);
			}
		}
	}
}

void Monster::clampPos()
{
	AARect clampRect = bloodworks->getMapLimits();
	clampRect.addThreshold(300.0f);
	clampRect.clampPos(position);
}

void Monster::spawnBits(const Vec2& blowDir, int extraBits)
{
	if (hasGibs == false || monsterTemplate->bodyPartBits.size() == 0 || (lastBitTime > timer.getTime() +  0.5f && extraBits == 0))
	{
		return;
	}

	lastBitTime = timer.getTime();

	int bitCount = randInt(0, 1) + extraBits;
	float blowAngle = blowDir.toAngle();
	for (int i = 0; i < bitCount; i++)
	{
		Vec2 dir = Vec2::fromAngle(blowAngle - 0.1f + 0.2f * randFloat());
		float r = randFloat() * 0.5f + 0.5f;
		r = sqrtf(r);
		int t = randInt(monsterTemplate->bodyPartBits.size());
		cTextureShr s = monsterTemplate->bodyPartBits[t];
		cTexturedQuadRenderable *partRenderable = new cTexturedQuadRenderable(bloodworks, s->getName(), "resources/default");
		partRenderable->setColor(renderable->getColor());
		Mat3 mat = renderable->getWorldMatrix();
		partRenderable->setWorldMatrix(mat);
		bloodworks->getBloodRenderable()->addBodyPart(partRenderable,
			position + dir * r * collisionRadius * 2.0f,
			s->getDimensions().toVec() * partScale * randFloat(0.7f, 1.2f),
			randFloat(0.0f, pi_2),
			blowDir * 4.0f * monsterTemplate->bitSpeed);
	}
}

int Monster::getId()
{
	return id;
}

void Monster::killSelf(const Vec2& blowDir)
{
	assert(isDead == false);
	isDead = true;

	if (luaKill)
	{
		luaKill(this);
	}

	bloodworks->onMonsterDied(this, dropChance);

	spawnBodyParts(blowDir);

	if (bloodworks->getPlayer())
	{
		bloodworks->getPlayer()->addScore((int)round(monsterTemplate->score * scoreMultiplier * randFloat(0.8f, 1.2f)));
		bloodworks->getPlayer()->gainExperience((int)(monsterTemplate->experience * experienceMultiplier * bloodworks->getPlayer()->getMonsterExperienceMultiplier()));
	}

	if (monsterTemplate->killSounds.size() && timer.getTime() - lastSoundTime > 0.1f && blowDir.isNonZero())
	{
		lastSoundTime = timer.getTime();
		cSoundSampleShr s = monsterTemplate->killSounds[randInt(monsterTemplate->killSounds.size())];
		bloodworks->playSoundAtMap(position, s, 0.7f);
	}
}
