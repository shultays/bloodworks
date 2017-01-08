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
#include "LaserRenderable.h"
#include "cShader.h"
#include "cRenderable.h"
#include "MonsterTemplate.h"
#include "cPostProcess.h"
#include "BuffFloat.h"
#include "LuaBuffController.h"

BloodworksLuaWorld::BloodworksLuaWorld(Bloodworks *b)
{
	this->bloodworks = b;
	
	buffController = nullptr;

#ifdef DEBUG
	lua["DEBUG"] = true;
#else
	lua["DEBUG"] = false;
#endif

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
		sol::meta_function::unary_minus, [](const Vec2& a) { return -a; },

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
		"copy",  [](const Vec2& a) { return Vec2(a.x, a.y); },

		"rotateBy", [](Vec2& a, float angle) { a = a * Mat2::rotation(angle); },

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
		sol::meta_function::unary_minus, [](const Vec3& a) { return -a; },


		"normalize", [](Vec3& a) { a.normalize(); },
		"normalized", [](const Vec3& a) { return a.normalized(); },

		"distance", [](const Vec3& a, const Vec3& b) { return a.distance(b); },
		"distanceSquared", [](const Vec3& a, const Vec3& b) { return a.distanceSquared(b); },

		"length", [](const Vec3& a) { return a.length(); },
		"lengthSquared", [](const Vec3& a) { return a.lengthSquared(); },

		"dot", [](const Vec3& a, const Vec3& b) { return a.dot(b); }
	);



	lua.new_usertype<Vec4>("Vec4",
		sol::constructors<sol::types<>, sol::types<float, float, float, float>>(),
		"x", &Vec4::x,
		"y", &Vec4::y,
		"z", &Vec4::z,
		"o", &Vec4::o,

		sol::meta_function::addition, [](const Vec4& a, const Vec4& b) { return a + b; },
		sol::meta_function::subtraction, [](const Vec4& a, const Vec4& b) { return a - b; },
		sol::meta_function::multiplication, [](const Vec4& a, const Vec4& b) { return a * b; },
		sol::meta_function::multiplication, [](const Vec4& a, float b) { return a * b; },
		sol::meta_function::division, [](const Vec4& a, const Vec4& b) { return a / b; },
		sol::meta_function::division, [](const Vec4& a, float b) { return a / b; },
		sol::meta_function::unary_minus, [](const Vec4& a) { return -a; },


		"normalize", [](Vec4& a) { a.normalize(); },
		"normalized", [](const Vec4& a) { return a.normalized(); },

		"distance", [](const Vec4& a, const Vec4& b) { return a.distance(b); },
		"distanceSquared", [](const Vec4& a, const Vec4& b) { return a.distanceSquared(b); },

		"length", [](const Vec4& a) { return a.length(); },
		"lengthSquared", [](const Vec4& a) { return a.lengthSquared(); },

		"dot", [](const Vec4& a, const Vec4& b) { return a.dot(b); }
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

	lua.new_usertype<LaserRenderable>("Laser",
		"setLevel", &LaserRenderable::setLevel,
		"setAllignment", &LaserRenderable::setAlignment,
		"setVisible", &LaserRenderable::setVisible,
		"setColor", &LaserRenderable::setColor,
		"setLaserData", &LaserRenderable::setLaserData,
		"setPositionAndAngle", &LaserRenderable::setPositionAndAngle,
		"setPosition", &LaserRenderable::setPosition,
		"setAngle", &LaserRenderable::setAngle,
		"setLength", &LaserRenderable::setLength,

		"addUniformFloat", &LaserRenderable::addUniformFloat,
		"addUniformVec2", &LaserRenderable::addUniformVec2,
		"addUniformVec3", &LaserRenderable::addUniformVec3,
		"addUniformVec4", &LaserRenderable::addUniformVec4
		);

	lua.new_usertype<cParticle>("Particle",
		"setLevel", &cParticle::setLevel,
		"setAllignment", &cParticle::setAlignment,
		"setVisible", &cParticle::setVisible,
		"setColor", &cParticle::setColor,
		"setWorldMatrix", &cParticle::setWorldMatrix,
		"addParticle", &cParticle::addParticle,
		"args", &cParticle::args,

		"setTexture", &cParticle::setTexture,
		"addTexture", &cParticle::addTexture
		);


	lua.new_usertype<cPostProcess>("PostProcess",
		"isEnabled", &cPostProcess::isEnabled,
		"setEnabled", &cPostProcess::setEnabled,
		"setShaderWeight", &cPostProcess::setShaderWeight,
		"addUniformFloat", &cPostProcess::addUniformFloat,
		"addUniformVec2", &cPostProcess::addUniformVec2,
		"addUniformVec3", &cPostProcess::addUniformVec3,
		"addUniformVec4", &cPostProcess::addUniformVec4
		);


	lua.new_usertype<Bullet>("Bullet",
		"index", sol::readonly(&Bullet::id),

		"position", sol::property(&Bullet::getPosition, &Bullet::setPosition),
		"moveSpeed", &Bullet::moveSpeed,
		"moveAngle", &Bullet::moveAngle,
		"meshRotation", &Bullet::meshRotation,

		"meshScale", &Bullet::meshScale,

		"moveDir", sol::readonly(&Bullet::moveDir),
		"moveSpeedDir", sol::readonly(&Bullet::moveSpeedDir),

		"radius", &Bullet::radius,
		"damage", &Bullet::damage,

		"penetrateCount", &Bullet::penetrateCount,
		"penetrateUsed", sol::readonly(&Bullet::penetrateUsed),

		"script", &Bullet::script,

		"onHitCallback", &Bullet::onHitCallback,
		"onTickCallback", &Bullet::onTickCallback,
		"shouldHitMonsterTest", &Bullet::shouldHitMonsterTest,

		"isDead", sol::readonly(&Bullet::isDead),

		"data", &Bullet::data,
		"onDamageArgs", &Bullet::onDamageArgs,

		"setPosition", &Bullet::setPosition,

		"addRenderableTexture", &Bullet::addRenderableTexture,
		"addRenderableTextureWithSize", &Bullet::addRenderableTextureWithSize,
		"addRenderableTextureWithPosAndSize", &Bullet::addRenderableTextureWithPosAndSize,
		"addTrailParticle", &Bullet::addTrailParticle,

		"updateDrawable", &Bullet::updateDrawable,
		"removeSelf", &Bullet::removeSelf
		);


	lua.set_function("addPostProcess",
		[&](const std::string& postProcessShader) -> cPostProcess*
	{
		cPostProcess *postProcess = new cPostProcess();
		postProcess->init(bloodworks, resources.getShader("resources/post_process/default.vs", postProcessShader), 100);
		return postProcess;
	});

	lua.set_function("removePostProcess",
		[&](cPostProcess *postProcess) 
	{
		bloodworks->removePostProcess(postProcess);
		SAFE_DELETE(postProcess);
	});

	lua.set_function("approachAngle",
		[&](float angle, float angleToApproach, float maxRotation) -> float
	{
		return approachAngle(angle, angleToApproach, maxRotation);
	});

	lua.set_function("isKeyReleased",
		[&](int key) -> bool
	{
		return input.isKeyReleased(key);
	});

	lua.set_function("isKeyPressed",
		[&](int key) -> bool
	{
		return input.isKeyPressed(key);
	});
	lua.set_function("isKeyUp",
		[&](int key) -> bool
	{
		return input.isKeyUp(key);
	});

	lua.set_function("isKeyDown",
		[&](int key) -> bool
	{
		return input.isKeyDown(key);
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

	lua.set_function("getUniqueId", [&]()
	{
		return bloodworks->getUniqueId();
	});

	lua.set_function("addLine",
		[&](const Vec2& pos0, const Vec2& pos1)
	{
		debugRenderer.addLine(pos0, pos1);
	});

	lua.set_function("addCircle",
		[&](const Vec2& pos0, float r)
	{
		debugRenderer.addCircle(pos0, r);
	});

	lua.set_function("addExplosion",
		[&](const Vec2& pos, float scale, float speed, int minDamage, int maxDamage)
	{
		bloodworks->addExplosion(pos, scale, speed, minDamage, maxDamage);
	});

	lua.set_function("playSound",
		[&](sol::table& args)
	{
		bloodworks->playOneShotSound(args);
	});


	lua.new_usertype<Gun>("Gun",
		"index", sol::readonly(&Gun::id),
		"data", &Gun::data,
		"name", &Gun::name,
		"bulletSpeed", &Gun::bulletSpeed,
		"bulletRadius", &Gun::bulletRadius,
		"bulletSpeed", &Gun::bulletSpeed,
		"scriptTable", &Gun::scriptTable,

		"spreadAngle", &Gun::spreadAngle,
		"crosshairDistance", &Gun::crosshairDistance,

		"isTriggered", &Gun::isTriggered,

		"onBulletHit", &Gun::onBulletHit,

		"getRandomDamage", &Gun::getRandomDamage,

		"hasAmmo", &Gun::hasAmmo,
		"addAmmo", &Gun::addAmmo,
		"consumeAmmo", &Gun::consumeAmmo,
		"reload", &Gun::reload,
		"isReloading", &Gun::isReloading,
		"getReloadPercentage", &Gun::getReloadPercentage,
		"getMaxAmmo", &Gun::getMaxAmmo,
		"getCurrentAmmo", &Gun::getCurrentAmmo,
		"reloadSpeedMultiplier", &Gun::reloadSpeedMultiplier,

		"addBullet", &Gun::addBullet,
		"laser", &Gun::laser
		);



	lua.set_function("getGunCount",
		[&]() -> int
	{
		return (int)bloodworks->getGuns().size();
	});

	lua.set_function("getGunAtIndex",
		[&](int index) -> Gun*
	{
		return bloodworks->getGuns()[index];
	});

	lua.set_function("isPaused",
		[&]() -> bool
	{
		return bloodworks->isPaused();
	});

	lua.set_function("loadMission",
		[&](const std::string& mission) -> bool
	{
		return bloodworks->loadMission(mission);
	});

	lua.set_function("gotoMainMenu",
		[&]() -> bool
	{
		return bloodworks->gotoMainMenu();
	});

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

	lua.set_function("getMonsterAtIndex",
		[&](int i) -> Monster*
	{
		return bloodworks->getMonsterController()->getMonsterAtIndex(i);
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

	lua.set_function("getClosestMonsterWithIgnoreId",
		[&](const Vec2& pos, int ignoreId) -> Monster*
	{
		return bloodworks->getMonsterController()->getClosestMonsterWithIgnoreId(pos, ignoreId);
	});

	lua.set_function("getClosestMonsterInRange",
		[&](const Vec2& pos, float range) -> Monster*
	{
		return bloodworks->getMonsterController()->getClosestMonsterInRange(pos, range);
	});

	lua.set_function("getRandomMapPosition",
		[&]() -> Vec2
	{
		return Vec2(randFloat(bloodworks->getMapMin().x + 50, bloodworks->getMapMax().x - 50.0f), randFloat(bloodworks->getMapMin().y + 50.0f, bloodworks->getMapMax().y - 50));
	});


	lua.set_function("getRandomPosition",
		[&](sol::table args) -> Vec2
	{
		return bloodworks->getMonsterController()->getRandomPos(args);
	});

	lua.set_function("getClosestMonsterInRangeWithIgnoreId",
		[&](const Vec2& pos, float range, int ignoreId) -> Monster*
	{
		return bloodworks->getMonsterController()->getClosestMonsterInRangeWithIgnoreId(pos, range, ignoreId);
	});

	lua.set_function("damageMonstersInRangeWithIgnoreId",
		[&](const Vec2& pos, float range, int minRange, int maxRange, bool mark, int ignoreId)
	{
		return bloodworks->getMonsterController()->damageMonstersInRangeWithIgnoreId(pos, range, minRange, maxRange, mark, ignoreId);
	});

	lua.new_usertype<MonsterController::MonsterHitResult>("MonsterHitResult",
		"monster", &MonsterController::MonsterHitResult::monster,
		"distance", &MonsterController::MonsterHitResult::distance
		);

	lua.set_function("getClosestMonsterOnLine",
		[&](const Vec2& pos, const Vec2& ray, int ignoreId)
	{
		return bloodworks->getMonsterController()->getClosestMonsterOnLine(pos, ray, ignoreId);
	});

	lua.new_usertype<MonsterTemplate>("MonsterTemplate",
		"name", sol::readonly(&MonsterTemplate::name),
		"experience", sol::readonly(&MonsterTemplate::experience),
		"textureSize", sol::readonly(&MonsterTemplate::size),
		"textureShift", sol::readonly(&MonsterTemplate::textureShift),
		"hitPoint", sol::readonly(&MonsterTemplate::hitPoint),
		"collisionRadius", sol::readonly(&MonsterTemplate::collisionRadius),
		"bulletRadius", sol::readonly(&MonsterTemplate::bulletRadius),
		"hasBlood", sol::readonly(&MonsterTemplate::hasBlood),
		"scriptPath", sol::readonly(&MonsterTemplate::scriptPath),
		"scriptTable", sol::readonly(&MonsterTemplate::scriptTable),
		"scriptArgs", sol::readonly(&MonsterTemplate::scriptArgs)
	);


	lua.new_usertype<Monster>("Monster",
		"name", &Monster::name,
		"index", sol::readonly(&Monster::id),

		"position", sol::property(&Monster::getPosition, &Monster::setPosition),
		"moveSpeed", &Monster::moveSpeed,
		"moveAngle", &Monster::moveAngle,
		"moveDir", sol::readonly(&Monster::moveDir),

		"textureShift", &Monster::textureShift,
		"textureSize", &Monster::textureSize,

		"hitPoint", &Monster::hitPoint,

		"collisionRadius", &Monster::collisionRadius,
		"bulletRadius", &Monster::bulletRadius,

		"data", &Monster::data,
		"experienceMultiplier", &Monster::experienceMultiplier,

		"scale", sol::readonly(&Monster::scale),

		"moveSpeedMultiplier", &Monster::moveSpeedMultiplier,
		"colorMultiplier", &Monster::colorMultiplier,

		"isDead", sol::readonly(&Monster::isDead),

		"playAnimation", &Monster::playAnimation,
		"addTimer", &Monster::addTimer,
		"setScale", &Monster::setScale,
		"setColor", &Monster::setColor,

		"doDamage", &Monster::doDamage,
		"doDamageWithArgs", &Monster::doDamageWithArgs,
		"addIgnoreId", &Monster::addIgnoreId,
		"hasIgnoreId", &Monster::hasIgnoreId,

		"monsterTemplate", sol::readonly(&Monster::monsterTemplate)
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
		"crosshairPos", sol::readonly(&Player::crosshairPos),
		"crosshairDistance", sol::readonly(&Player::crosshairDistance),
		"gunPos", sol::readonly(&Player::gunPos),
		"aimDir", sol::readonly(&Player::aimDir),
		"aimAngle", sol::readonly(&Player::aimAngle),
		"moveDir", sol::readonly(&Player::moveDir),
		"visible", sol::readonly(&Player::visible),
		"isDead", sol::readonly(&Player::isDead),
		"moveSpeedDir", sol::readonly(&Player::moveSpeedDir),
		"hitPoints", sol::readonly(&Player::hitPoints),
		"maxHitPoints", sol::readonly(&Player::maxHitPoints),
		"maxSpeed", sol::readonly(&Player::maxSpeed),
		"doDamage", &Player::doDamage,
		"doHeal", &Player::doHeal,
		"slowdown", &Player::slowdown,
		"gainExperience", &Player::gainExperience,
		"bulletSpeedMult", &Player::bulletSpeedMult,
		"shootSpeedMult", &Player::shootSpeedMult,
		"moveSpeedMult", &Player::moveSpeedMult,
		"slowdownOnHit", &Player::slowdownOnHit,
		"monsterExperienceMult", &Player::monsterExperienceMult,
		"damageMult", &Player::damageMult,
		"reloadSpeedMultiplier", &Player::reloadSpeedMultiplier,
		"globalMonsterSpeedMultiplier", &Player::globalMonsterSpeedMultiplier,
		"setGun", &Player::setGun,
		"level", sol::readonly(&Player::level),
		"gun", sol::readonly(&Player::gun),
		"experience", sol::readonly(&Player::experience),
		"experienceForNextLevel", sol::readonly(&Player::experienceForNextLevel),
		"data", &Player::data
		);

	lua.new_usertype<BuffFloat::BuffInfo>("BuffFloatInfo",
		"buffType", sol::readonly(&BuffFloat::BuffInfo::buffType),

		"buffAmount", sol::readonly(&BuffFloat::BuffInfo::buffAmount),
		"duration", sol::readonly(&BuffFloat::BuffInfo::duration),
		"fadeInDuration", sol::readonly(&BuffFloat::BuffInfo::fadeInDuration),
		"fadeOutDuration", sol::readonly(&BuffFloat::BuffInfo::fadeOutDuration),

		"restart", &BuffFloat::BuffInfo::restart,

		"removeAfterEnds", &BuffFloat::BuffInfo::removeAfterEnds,

		"setBuffAmount", &BuffFloat::BuffInfo::setBuffAmount,
		"setBuffDuration", &BuffFloat::BuffInfo::setBuffDuration,
		"getCurrentBuffAmount", &BuffFloat::BuffInfo::getCurrentBuffAmount,
		"setBuffFadeInFadeOut", &BuffFloat::BuffInfo::setBuffFadeInFadeOut

		);

	lua.new_usertype<BuffFloat>("BuffFloat",

		"create", sol::initializers([&](BuffFloat& memory)
	{
		new (&memory) BuffFloat();
		buffController->addBuff(&memory);
	}),
		sol::meta_function::garbage_collect, sol::destructor([&](BuffFloat& memory)
	{
		buffController->removeBuff(&memory);
		memory.~BuffFloat();
	}),

		"addBuffWithType", &BuffFloat::addBuffWithType,
		"addBuff", [&](BuffFloat& buff, float amount)
	{
		int id = bloodworks->getUniqueId();
		buff.addBuff(id, amount, BuffFloat::multiply_buff);
		return id;
	},

		"addBuffWithId", [&](BuffFloat& buff, int id, float amount)
	{
		buff.addBuff(id, amount, BuffFloat::multiply_buff);
		return id;
	},
		"setBaseValue", &BuffFloat::setBaseValue,
		"setBuffAmount", &BuffFloat::setBuffAmount,
		"setBuffDuration", &BuffFloat::setBuffDuration,
		"setBuffFadeInFadeOut", &BuffFloat::setBuffFadeInFadeOut,

		"getBuffInfo", &BuffFloat::getBuffInfo,
		"hasBuffInfo", &BuffFloat::hasBuffInfo,

		"getBaseValue", &BuffFloat::getBaseValue,
		"getBuffedValue", &BuffFloat::getBuffedValue

		);


	lua.new_usertype<BuffVec4::BuffInfo>("BuffVec4Info",
		"buffType", sol::readonly(&BuffVec4::BuffInfo::buffType),

		"buffAmount", sol::readonly(&BuffVec4::BuffInfo::buffAmount),
		"duration", sol::readonly(&BuffVec4::BuffInfo::duration),
		"fadeInDuration", sol::readonly(&BuffVec4::BuffInfo::fadeInDuration),
		"fadeOutDuration", sol::readonly(&BuffVec4::BuffInfo::fadeOutDuration),

		"restart", &BuffVec4::BuffInfo::restart,

		"removeAfterEnds", &BuffVec4::BuffInfo::removeAfterEnds,

		"setBuffAmount", &BuffVec4::BuffInfo::setBuffAmount,
		"setBuffDuration", &BuffVec4::BuffInfo::setBuffDuration,
		"getCurrentBuffAmount", &BuffVec4::BuffInfo::getCurrentBuffAmount,
		"setBuffFadeInFadeOut", &BuffVec4::BuffInfo::setBuffFadeInFadeOut

		);

	lua.new_usertype<BuffVec4>("BuffVec4",
		"addBuffWithType", &BuffVec4::addBuffWithType,
		"addBuff", [&](BuffVec4& buff, Vec4 amount)
	{
		int id = bloodworks->getUniqueId();
		buff.addBuff(id, amount, BuffVec4::multiply_buff);
		return id;
	},

		"addBuffWithId", [&](BuffVec4& buff, int id, Vec4 amount)
	{
		buff.addBuff(id, amount, BuffVec4::multiply_buff);
		return id;
	},
		"setBaseValue", &BuffVec4::setBaseValue,
		"setBuffAmount", &BuffVec4::setBuffAmount,
		"setBuffDuration", &BuffVec4::setBuffDuration,
		"setBuffFadeInFadeOut", &BuffVec4::setBuffFadeInFadeOut,

		"getBuffInfo", &BuffVec4::getBuffInfo,
		"hasBuffInfo", &BuffVec4::hasBuffInfo,

		"getBaseValue", &BuffVec4::getBaseValue,
		"getBuffedValue", &BuffVec4::getBuffedValue

		);
}


void BloodworksLuaWorld::reset()
{
	clear();
	buffController = new LuaBuffController();
}

void BloodworksLuaWorld::clear()
{
	SAFE_DELETE(buffController);
}

void BloodworksLuaWorld::tick()
{
	buffController->tick();
}
