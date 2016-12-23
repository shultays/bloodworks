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

Monster::Monster(Bloodworks *bloodworks)
{
	this->bloodworks = bloodworks;
	id = bloodworks->getUniqueId();
	addIgnoreId(id);
}

void Monster::init(const MonsterTemplate* monsterTemplate)
{
	this->monsterTemplate = monsterTemplate;
	name = monsterTemplate->name;

	isDead = false;

	textureSize = monsterTemplate->size;
	textureShift = monsterTemplate->textureShift;
	hitPoint = monsterTemplate->hitPoint;
	hasBlood = monsterTemplate->hasBlood;

	renderable = new cAnimatedTexturedQuadRenderable(bloodworks, "resources/default");
	renderable->addAnimation(monsterTemplate->animationData);
	bloodworks->addRenderable(renderable, MONSTERS);

	renderable->setDefaultAnimation(renderable->getAnimationIndex("stand"));

	healthRenderable = new cTextRenderable(bloodworks, resources.getFont("resources/fontSmallData.txt"), "", 10);
	healthRenderable->setTextAllignment(TextAlignment::center);
	bloodworks->addRenderable(healthRenderable, OBJECT_GUI);

	moveAngle = randFloat(-pi, pi);
	moveSpeed = 0.0f;

	data = lua.create_table_with();

	scriptTable = monsterTemplate->scriptTable;

	bulletRadius = monsterTemplate->bulletRadius;
	collisionRadius = monsterTemplate->collisionRadius;
	position = 100.0f;
	scale = 1.0f;
	scriptTable["init"](this);
}

void Monster::setScale(float scale)
{
	this->scale = scale;
	textureSize = monsterTemplate->size * scale;
	textureShift = monsterTemplate->textureShift * scale;
	bulletRadius = monsterTemplate->bulletRadius * scale;
	collisionRadius = monsterTemplate->collisionRadius * scale;
}

void Monster::setColor(int color)
{
	renderable->setColor(Vec4::fromColor(color));
}

Monster::~Monster()
{
	SAFE_DELETE(renderable);
	SAFE_DELETE(healthRenderable);
}

void Monster::tick()
{
	scriptTable["onTick"](this);
	healthRenderable->setVisible(input.isKeyDown(key_f6));
	std::vector<int> toTrigger;
	for (int i = (int)timers.size() - 1; i >= 0; i--)
	{
		if (timers[i].time < timer.getTime())
		{
			toTrigger.push_back(i);
		}
	}

	for (auto& i : toTrigger)
	{
		Timer t = timers[i];
		timers[i] = timers[(int)timers.size() - 1];
		timers.resize(timers.size() - 1);
		scriptTable[t.func](t.args);
	}

	if (moveSpeed > 0.0f)
	{
		position += Vec2::fromAngle(moveAngle) * moveSpeed * timer.getDt();
	}

	std::stringstream ss;
	ss << (int)hitPoint;

	healthRenderable->setText(ss.str());
	healthRenderable->setWorldMatrix(Mat3::translationMatrix(position + Vec2(0.0f, bulletRadius + 10.0f)));
	Mat3 mat = Mat3::identity();
	mat.scaleBy(textureSize);
	mat.translateBy(textureShift);
	mat.rotateBy(-pi_d2 - moveAngle);
	mat.translateBy(position);
	renderable->setWorldMatrix(mat);

	//debugRenderer.addLine(position, position + Vec2::fromAngle(moveAngle) * 50);
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
void Monster::doDamageWithArgs(int damage, const Vec2& dir, sol::table& args)
{
	if (isDead)
	{
		return;
	}
	hitPoint -= damage;
	if (hitPoint <= 1.0f)
	{
		killSelf(dir * clamped(damage * 0.3f, 0.0f, 20.0f));
	}
	else
	{
		bloodworks->getBloodRenderable()->addBlood(position, dir * clamped(damage * 0.3f, 0.0f, 20.0f), 10.0f);
		if (scriptTable["onHit"])
		{
			scriptTable["onHit"](this, damage, args);
		}
	}
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
		int i = (int)ignoreIds.size() - 1;
		while (i > 0 && ignoreIds[i - 1] > id)
		{
			ignoreIds[i] = ignoreIds[i - 1];
			i--;
		}
		ignoreIds[i] = id;
	}
}

bool Monster::hasIgnoreId(int id)
{
	return std::binary_search(ignoreIds.begin(), ignoreIds.end(), id);
}

int Monster::getId()
{
	return id;
}

void Monster::killSelf(const Vec2& blowDir)
{
	assert(isDead == false);
	isDead = true;
	if (scriptTable["onKilled"])
	{
		scriptTable["onKilled"](this);
	}
	bloodworks->getBloodRenderable()->addBlood(position, blowDir, 18.0f);

	if (randFloat() < 0.1f || input.isKeyDown(key_1))
	{
		bloodworks->addDrop(position);
	}

	std::vector<int> parts;
	int maxCount = (int)monsterTemplate->bodyParts.size();
	int partCount = randInt(2, maxCount - 2);
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
	for (int i : parts)
	{
		cTextureShr s = monsterTemplate->bodyParts[i].texture;
		cTexturedQuadRenderable *partRenderable = new cTexturedQuadRenderable(bloodworks, s->getName(), "resources/default");
		partRenderable->setColor(renderable->getColor());
		Mat3 mat = renderable->getWorldMatrix();
		partRenderable->setWorldMatrix(mat);
		bloodworks->getBloodRenderable()->addBodyPart(partRenderable,
			position - (monsterTemplate->bodyParts[i].shift * scale) * Mat2::rotation(moveAngle - pi_d2),
			textureSize,
			moveAngle - pi_d2, 
			textureShift + monsterTemplate->bodyParts[i].shift * scale, 
			blowDir * 2.0f);
	}

	if (bloodworks->getPlayer())
	{
		bloodworks->getPlayer()->gainExperience(monsterTemplate->experience);
	}
}
