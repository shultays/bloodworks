#include "BloodworksLuaWorld.h"

#include "Bloodworks.h"
#include "cGlobals.h"
#include "Bullet.h"
#include "Monster.h"
#include "Gun.h"
#include "MonsterController.h"
#include "BulletController.h"
#include "MissionController.h"
#include "cParticle.h"
#include "cRenderable.h"
#include "GameObject.h"
#include "Player.h"

BloodworksLuaWorld::BloodworksLuaWorld(Bloodworks *b)
{
	this->bloodworks = b;
	lua.new_usertype<Vec2>("Vec2",
		sol::constructors<sol::types<>, sol::types<float, float>>(),
		"x", &Vec2::x,
		"y", &Vec2::y,

		sol::meta_function::addition, [](const Vec2& a, const Vec2& b) { return a + b; },
		sol::meta_function::subtraction, [](const Vec2& a, const Vec2& b) { return a - b; },
		sol::meta_function::multiplication, [](const Vec2& a, const Vec2& b) { return a * b; },
		sol::meta_function::multiplication, [](const Vec2& a, float b) { return a * b; },
		sol::meta_function::division, [](const Vec2& a, const Vec2& b) { return a / b; },
		sol::meta_function::division, [](const Vec2& a, float b) { return a / b; },

		"setAngle", [](Vec2& v, float angle) { v = Vec2::fromAngle(angle); },
		"getAngle", &Vec2::toAngle,

		"normalize", [](Vec2& a) { return a.normalize(); },
		"normalized", [](const Vec2& a) { return a.normalized(); },

		"safeNormalize", [](Vec2& a) { return a.safeNormalize(); },
		"safeNormalized", [](const Vec2& a) { return a.safeNormalized(); },

		"distance", [](const Vec2& a, const Vec2& b) { return a.distance(b); },
		"distanceSquared", [](const Vec2& a, const Vec2& b) { return a.distanceSquared(b); },

		"length", [](const Vec2& a) { return a.length(); },
		"lengthSquared", [](const Vec2& a) { return a.lengthSquared(); },

		"sideVec", &Vec2::sideVec,

		"dot", [](const Vec2& a, const Vec2& b) { return a.dot(b); }
	);


	lua.new_usertype<Vec3>("Vec3",
		sol::constructors<sol::types<>, sol::types<float, float, float>>(),
		"x", &Vec3::x,
		"y", &Vec3::y,
		"z", &Vec3::z,

		sol::meta_function::addition, [](const Vec3& a, const Vec3& b) { return a + b; },
		sol::meta_function::subtraction, [](const Vec3& a, const Vec3& b) { return a - b; },
		sol::meta_function::multiplication, [](const Vec3& a, const Vec3& b) { return a * b; },
		sol::meta_function::multiplication, [](const Vec3& a, float b) { return a * b; },
		sol::meta_function::division, [](const Vec3& a, const Vec3& b) { return a / b; },
		sol::meta_function::division, [](const Vec3& a, float b) { return a / b; },


		"normalize", [](Vec3& a) { a.normalize(); },
		"normalized", [](const Vec3& a) { return a.normalized(); },

		"distance", [](const Vec3& a, const Vec3& b) { return a.distance(b); },
		"distanceSquared", [](const Vec3& a, const Vec3& b) { return a.distanceSquared(b); },

		"length", [](const Vec3& a) { return a.length(); },
		"lengthSquared", [](const Vec3& a) { return a.lengthSquared(); },

		"dot", [](const Vec3& a, const Vec3& b) { return a.dot(b); }
	);

	lua.new_usertype<Mat3>("Mat3",
		"fromPosition", [](const Vec2& pos) { return Mat3::translationMatrix(pos); },
		"fromPositionAndScale", [](const Vec2& pos, const Vec2& scale) { return Mat3::scaleMatrix(scale).translateBy(pos); },
		"from", [](const Vec2& pos, const Vec2& scale, float rotation) { return Mat3::scaleMatrix(scale).rotateBy(rotation).translateBy(pos); }
	);

	lua.new_usertype<cRenderable>("Renderable",
		"setLevel", &cRenderable::setLevel,
		"setAllignment", &cRenderable::setAlignment,
		"setVisible", &cRenderable::setVisible,
		"setColor", &cRenderable::setColor,
		"setWorldMatrix", &cRenderable::setWorldMatrix
		);

	lua.new_usertype<cParticle>("Particle",
		"setLevel", &cParticle::setLevel,
		"setAllignment", &cParticle::setAlignment,
		"setVisible", &cParticle::setVisible,
		"setColor", &cParticle::setColor,
		"setWorldMatrix", &cParticle::setWorldMatrix,
		"addParticle", &cParticle::addParticle,
		"args", &cParticle::args
		);

	lua.new_usertype<Bullet>("Bullet",
		"index", sol::readonly(&Bullet::id),

		"position", &Bullet::pos,
		"moveSpeed", &Bullet::moveSpeed,
		"moveAngle", &Bullet::moveAngle,
		"meshRotation", &Bullet::meshRotation,

		"meshScale", &Bullet::meshScale,

		"moveDir", sol::readonly(&Bullet::moveDir),
		"moveSpeedDir", sol::readonly(&Bullet::moveSpeedDir),

		"radius", &Bullet::radius,
		"damage", &Bullet::damage,

		"diesOnHit", &Bullet::diesOnHit,
		"updateDrawable", &Bullet::updateDrawable,

		"script", &Bullet::script,

		"onHitCallback", &Bullet::onHitCallback,
		"onTickCallback", &Bullet::onTickCallback,
		"shouldHitMonsterTest", &Bullet::shouldHitMonsterTest,

		"isDead", sol::readonly(&Bullet::isDead),

		"data", &Bullet::data,

		"addRenderableTexture", &Bullet::addRenderableTexture,
		"addRenderableTextureWithSize", &Bullet::addRenderableTextureWithSize,
		"addRenderableTextureWithPosAndSize", &Bullet::addRenderableTextureWithPosAndSize,
		"addTrailParticle", &Bullet::addTrailParticle,

		"removeSelf", &Bullet::removeSelf
		);

	lua.set_function("approachAngle",
		[&](float angle, float angleToApproach, float maxRotation) -> float
	{
		return approachAngle(angle, angleToApproach, maxRotation);
	});

	lua.set_function("angleDiff",
		[&](float angle, float angleToApproach) -> float
	{
		return angleDiff(angle, angleToApproach);
	});

	lua.set_function("multiplyGameSpeed", [&](float multiplier)
	{
		bloodworks->multiplyGameSpeed(multiplier);
	});

	lua.set_function("addLine",
		[&](const Vec2& pos0, const Vec2& pos1)
	{
		debugRenderer.addLine(pos0, pos1);
	});

	lua.set_function("addExplosion",
		[&](const Vec2& pos, float scale, float speed, int minDamage, int maxDamage)
	{
		bloodworks->addExplosion(pos, scale, speed, minDamage, maxDamage);
	});


	lua.new_usertype<Gun>("Gun",
		"index", sol::readonly(&Gun::id),
		"data", &Gun::data,

		"bulletSpeed", &Gun::bulletSpeed,
		"bulletRadius", &Gun::bulletRadius,
		"bulletSpeed", &Gun::bulletSpeed,
		"scriptTable", &Gun::scriptTable,

		"spreadAngle", &Gun::spreadAngle,
		"crosshairDistance", &Gun::crosshairDistance,

		"isTriggered", &Gun::isTriggered,

		"addBullet", &Gun::addBullet
		);

	lua.set_function("addGameObject",
		[&](const std::string& script) -> GameObject*
	{
		return bloodworks->getMissionController()->addGameObject(script);
	});

	lua.set_function("addCustomBullet",
		[&](const sol::table& params) -> Bullet*
	{
		return bloodworks->getBulletController()->addCustomBullet(params);
	});


	lua.set_function("addMonster",
		[&](std::string monsterTemplate) -> Monster*
	{
		return bloodworks->getMonsterController()->addMonster(monsterTemplate);
	});

	lua.set_function("getMonsterCount",
		[&]() -> int
	{
		return bloodworks->getMonsterController()->getMonsterCount();
	});

	lua.set_function("getMonster",
		[&](int i) -> Monster*
	{
		return bloodworks->getMonsterController()->getMonster(i);
	});

	lua.set_function("getClosestMonster",
		[&](const Vec2& pos) -> Monster*
	{
		return bloodworks->getMonsterController()->getClosestMonster(pos);
	});

	lua.set_function("getClosestMonsterWithIgnoreData",
		[&](const Vec2& pos, const std::string& ignoreData) -> Monster*
	{
		return bloodworks->getMonsterController()->getClosestMonsterWithIgnoreData(pos, ignoreData);
	});

	lua.set_function("getClosestMonsterInRange",
		[&](const Vec2& pos, float range) -> Monster*
	{
		return bloodworks->getMonsterController()->getClosestMonsterInRange(pos, range);
	});

	lua.set_function("getClosestMonsterInRangeWithIgnoreData",
		[&](const Vec2& pos, float range, const std::string& ignoreData) -> Monster*
	{
		return bloodworks->getMonsterController()->getClosestMonsterInRangeWithIgnoreData(pos, range, ignoreData);
	});

	lua.set_function("damageMonstersInRangeWithIgnoreData",
		[&](const Vec2& pos, float range, int minRange, int maxRange, bool mark, const std::string& ignoreData)
	{
		return bloodworks->getMonsterController()->damageMonstersInRangeWithIgnoreData(pos, range, minRange, maxRange, mark, ignoreData);
	});

	lua.new_usertype<Monster>("Monster",
		"name", &Monster::name,
		"index", sol::readonly(&Monster::id),

		"position", &Monster::position,
		"moveSpeed", &Monster::moveSpeed,
		"moveAngle", &Monster::moveAngle,

		"textureShift", &Monster::textureShift,
		"textureSize", &Monster::textureSize,

		"hitPoint", &Monster::hitPoint,

		"collisionRadius", &Monster::collisionRadius,
		"bulletRadius", &Monster::bulletRadius,

		"data", &Monster::data,

		"isDead", sol::readonly(&Monster::isDead),

		"playAnimation", &Monster::playAnimation,
		"addTimer", &Monster::addTimer,
		"setScale", &Monster::setScale,
		"setColor", &Monster::setColor,

		"doDamage", &Monster::doDamage
		);



	lua.new_usertype<GameObject>("GameObject",
		"index", sol::readonly(&GameObject::id),

		"toBeRemoved", &GameObject::toBeRemoved,
		"script", &GameObject::script,
		"data", &GameObject::data,
		"addText", &GameObject::addText,
		"addTexture", &GameObject::addTexture,

		"setPosition", &GameObject::setPosition,
		"setRotation", &GameObject::setRotation,
		"setScale", &GameObject::setScale,
		"setLevel", &GameObject::setLevel
		);


	lua.new_usertype<GameObject::RenderableData>("RenderableData",
		"position", &GameObject::RenderableData::pos,
		"rotation", &GameObject::RenderableData::rotation,
		"textureSize", &GameObject::RenderableData::textureSize,
		"textSize", &GameObject::RenderableData::textSize,
		"color", &GameObject::RenderableData::color,
		"update", &GameObject::RenderableData::update,
		"alignment", &GameObject::RenderableData::alignment,
		"textAlignment", &GameObject::RenderableData::textAlignment
		);


	lua.new_usertype<Player>("Player",
		"position", &Player::pos,
		"moveSpeed", &Player::moveSpeed,
		"moveAngle", &Player::moveAngle,
		"crosshairPos", &Player::crosshairPos,
		"aimDir", sol::readonly(&Player::aimDir),
		"moveDir", sol::readonly(&Player::moveDir),
		"moveSpeedDir", sol::readonly(&Player::moveSpeedDir),
		"hitPoints", sol::readonly(&Player::hitPoints),
		"doDamage", &Player::doDamage,
		"doHeal", &Player::doHeal,
		"slowdown", &Player::slowdown,
		"gainExperience", &Player::gainExperience,
		"bulletSpeedMult", &Player::bulletSpeedMult,
		"shootSpeedMult", &Player::shootSpeedMult,
		"moveSpeedMult", &Player::moveSpeedMult,
		"slowdownOnHit", &Player::slowdownOnHit,


		"level", sol::readonly(&Player::level),
		"experience", sol::readonly(&Player::experience),
		"experienceForNextLevel", sol::readonly(&Player::experienceForNextLevel),
		"data", &Player::data
		);

}
