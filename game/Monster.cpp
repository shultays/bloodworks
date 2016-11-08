#include "Monster.h"
#include "Bloodworks.h"
#include "cAnimatedRenderable.h"
#include "cGlobals.h"
#include "cTools.h"
#include "cFont.h"
#include "BloodRenderable.h"

int Monster::nextId = 0;

Monster::Monster(Bloodworks *bloodworks)
{
	this->bloodworks = bloodworks;
	index = nextId++;
}

void Monster::init(const MonsterTemplate* monsterTemplate)
{
	this->monsterTemplate = monsterTemplate;
	name = monsterTemplate->name;

	isDead = false;

	size = monsterTemplate->size;
	textureShift = monsterTemplate->textureShift;
	hitPoint = monsterTemplate->hitPoint;
	hasBlood = monsterTemplate->hasBlood;

	renderable = new cAnimatedTexturedQuadRenderable(bloodworks, "resources/default");
	renderable->addAnimation(monsterTemplate->animationData);
	bloodworks->addRenderable(renderable, 100);

	if (monsterTemplate->defaultAnimation.size())
	{
		renderable->setDefaultAnimation(renderable->getAnimationIndex(monsterTemplate->defaultAnimation));
	}

	healthRenderable = new cTextRenderable(bloodworks, resources.getFont("resources/fontSmallData.txt"), "", 10);
	bloodworks->addRenderable(healthRenderable, 101);

	renderable->playAnimation("walk", randFloat());

	moveAngle = randFloat(-pi, pi);
	moveSpeed = 0.0f;


	auto monsters = lua["monsters"];
	luaMonster = monsters[index] = lua.create_table_with();

	scriptTable = monsterTemplate->scriptTable;

	bulletRadius = monsterTemplate->bulletRadius;
	collisionRadius = monsterTemplate->collisionRadius;
	position = 100.0f;
	reset();
}

void Monster::reset()
{
	lua["monsterId"] = index;
	scriptTable["init"](false);
	if (luaMonster["scale"])
	{
		float scale = luaMonster["scale"].get<float>();
		size = monsterTemplate->size * scale;
		textureShift = monsterTemplate->textureShift * scale;
		bulletRadius = monsterTemplate->bulletRadius * scale;
		collisionRadius = monsterTemplate->collisionRadius * scale;
	}

	if (luaMonster["color"])
	{
		renderable->setColor(Vec4::fromColor(luaMonster["color"].get<int>()));
	}

	healthRenderable->setPosition(position + Vec2(0.0f, bulletRadius + 10.0f));

	position = 100.0f;
	getMonsterData();
}

Monster::~Monster()
{
	SAFE_DELETE(renderable);
	SAFE_DELETE(healthRenderable);
}

void Monster::tick(float dt)
{
	lua["monsterId"] = index;
	scriptTable["onTick"](dt);

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

	getMonsterData();
	if (moveSpeed > 0.0f)
	{
		position += Vec2::fromAngle(moveAngle) * moveSpeed * dt;
		setMonsterData();
	}

	std::stringstream ss;
	ss << (int)hitPoint;

	healthRenderable->setText(ss.str());
	healthRenderable->setPosition(position + Vec2(-15 + (hitPoint<100 ? 5.0f : 0.0f) + (hitPoint<10 ? 5.0f : 0.0f), bulletRadius + 10.0f));
	Mat3 mat = Mat3::identity();
	mat.scaleBy(size);
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

void Monster::playAnimation(const std::string& anim)
{
	renderable->playAnimation(anim);
}

void Monster::doDamage(int damage)
{
	hitPoint -= damage;
	if (hitPoint <= 1.0f)
	{
		killSelf();
	}
}

int Monster::getId()
{
	return index;
}

void Monster::killSelf()
{
	assert(isDead == false);
	isDead = true;
	if (scriptTable["onKilled"])
	{
		scriptTable["onKilled"]();
	}
	bloodworks->getBloodRenderable()->addBlood(position);

	if (randFloat() < 0.1f || input.isKeyDown(key_1))
	{
		bloodworks->addDrop(position);
	}
}

void Monster::setMonsterData()
{
	luaMonster["x"] = position.x;
	luaMonster["y"] = position.y;
	luaMonster["moveAngle"] = moveAngle;
	luaMonster["moveSpeed"] = moveSpeed;
}

void Monster::getMonsterData()
{
	position.x = luaMonster["x"];
	position.y = luaMonster["y"];
	moveAngle = luaMonster["moveAngle"];
	moveSpeed = luaMonster["moveSpeed"];
}
