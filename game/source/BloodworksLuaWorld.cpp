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
#include "Bonus.h"
#include "DropController.h"
#include "Bonus.h"
#include "cTimeProfiler.h"
#include "cAnimatedRenderable.h"
#include "cTextRenderable.h"
#include "cTexturedQuadRenderable.h"
#include "GameObjectTemplate.h"
#include "CollisionController.h"
#include "cCircle.h"
#include "cRect.h"
#include "cCapsule.h"
#include "Perk.h"

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

		"fromAngle", [](float angle) { return Vec2::fromAngle(angle); },

		"randDir", []() { return Vec2::fromAngle(randFloat(pi_2)); },

		"rotateBy", [](Vec2& v, float angle) { v = v * Mat2::rotation(angle); },

		"setAngle", [](Vec2& v, float angle) { v = Vec2::fromAngle(angle); },
		"getAngle", &Vec2::toAngle,

		"normalize", [](Vec2& a) { return a.normalize(); },
		"normalized", [](const Vec2& a) { return a.normalized(); },
		"copy", [](const Vec2& a) { return Vec2(a.x, a.y); },

		"safeNormalize", [](Vec2& a) { return a.safeNormalize(); },
		"safeNormalized", [](const Vec2& a) { return a.safeNormalized(); },

		"distance", [](const Vec2& a, const Vec2& b) { return a.distance(b); },
		"distanceSquared", [](const Vec2& a, const Vec2& b) { return a.distanceSquared(b); },

		"length", [](const Vec2& a) { return a.length(); },
		"lengthSquared", [](const Vec2& a) { return a.lengthSquared(); },

		"sideVec", &Vec2::sideVec,
		"copy", [](const Vec2& a) { return Vec2(a.x, a.y); },

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

		"fromColor", [](unsigned int color) { return Vec4::fromColor(color); },

		"normalize", [](Vec4& a) { a.normalize(); },
		"normalized", [](const Vec4& a) { return a.normalized(); },

		"distance", [](const Vec4& a, const Vec4& b) { return a.distance(b); },
		"distanceSquared", [](const Vec4& a, const Vec4& b) { return a.distanceSquared(b); },

		"length", [](const Vec4& a) { return a.length(); },
		"lengthSquared", [](const Vec4& a) { return a.lengthSquared(); },

		"dot", [](const Vec4& a, const Vec4& b) { return a.dot(b); }
	);

	lua.new_usertype<Mat3>("Mat3",
		sol::meta_function::multiplication, [](const Mat3& a, const Mat3& b) { return a * b; },
		"fromScale", [](float x, float y) { return Mat3::scaleMatrix(x, y); },
		"fromRotation", [](float rotation) { return Mat3::rotationMatrix(rotation); },
		"fromPosition", [](float x, float y) { return Mat3::translationMatrix(x, y); },
		"fromPositionAndScale", [](const Vec2& pos, const Vec2& scale) { return Mat3::scaleMatrix(scale).translateBy(pos); },
		"fromScaleAndRotation", [](const Vec2& scale, float rotation) { return Mat3::scaleMatrix(scale).rotateBy(rotation); },
		"fromPositionAndRotation", [](const Vec2& pos, float rotation) { return Mat3::rotationMatrix(rotation).translateBy(pos); },
		"from", [](const Vec2& pos, const Vec2& scale, float rotation) { return Mat3::scaleMatrix(scale).rotateBy(rotation).translateBy(pos); }
	);

	lua.new_usertype<cRenderable>("Renderable",
		"setLevel", &cRenderable::setLevel,
		"setAlignment", &cRenderable::setAlignment,
		"setVisible", &cRenderable::setVisible,
		"setColor", &cRenderable::setColor,
		"setWorldMatrix", &cRenderable::setWorldMatrix
		);

	lua.new_usertype<LaserRenderable>("Laser",
		"setLevel", &LaserRenderable::setLevel,
		"setAlignment", &LaserRenderable::setAlignment,
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


	lua.new_usertype<cTextRenderable>("TextRenderable",
		"setLevel", &cTextRenderable::setLevel,
		"setAlignment", &cTextRenderable::setAlignment,
		"setVisible", &cTextRenderable::setVisible,
		"setColor", &cTextRenderable::setColor,
		"setWorldMatrix", &cTextRenderable::setWorldMatrix,
		"setText", &cTextRenderable::setText,
		"setVerticalTextAlignment", &cTextRenderable::setVerticalTextAlignment,
		"setTextAlignment", &cTextRenderable::setTextAlignment,
		"setTextSize", &cTextRenderable::setTextSize,
		"setMaxLength", &cTextRenderable::setMaxLength,
		"setMaxLineCount", &cTextRenderable::setMaxLineCount
		);

	lua.new_usertype<cTexturedQuadRenderable>("TextureRenderable",
		"setLevel", &cTexturedQuadRenderable::setLevel,
		"setAlignment", &cTexturedQuadRenderable::setAlignment,
		"setVisible", &cTexturedQuadRenderable::setVisible,
		"setColor", &cTexturedQuadRenderable::setColor,
		"setWorldMatrix", &cTexturedQuadRenderable::setWorldMatrix,

		"setUniformFloat", [](cTexturedQuadRenderable* particle, const std::string& name, float val)
	{
		particle->addUniformFloat(name, val);
	},
		"setUniformVec2", [](cTexturedQuadRenderable* particle, const std::string& name, const Vec2& val)
	{
		particle->addUniformVec2(name, val);
	},
		"setUniformVec3", [](cTexturedQuadRenderable* particle, const std::string& name, const Vec3& val)
	{
		particle->addUniformVec3(name, val);
	},
		"setUniformVec4", [](cTexturedQuadRenderable* particle, const std::string& name, const Vec4& val)
	{
		particle->addUniformVec4(name, val);
	},

		"setUniformInt", [](cTexturedQuadRenderable* particle, const std::string& name, int val)
	{
		particle->addUniformInt(name, val);
	},
		"setUniformIntVec2", [](cTexturedQuadRenderable* particle, const std::string& name, const IntVec2& val)
	{
		particle->addUniformIntVec2(name, val);
	},
		"setUniformIntVec3", [](cTexturedQuadRenderable* particle, const std::string& name, const IntVec3& val)
	{
		particle->addUniformIntVec3(name, val);
	},
		"setUniformIntVec4", [](cTexturedQuadRenderable* particle, const std::string& name, const IntVec4& val)
	{
		particle->addUniformIntVec4(name, val);
	}
		);

	lua.new_usertype<cAnimatedTexturedQuadRenderable>("Animation",
		"setLevel", &cAnimatedTexturedQuadRenderable::setLevel,
		"setAlignment", &cAnimatedTexturedQuadRenderable::setAlignment,
		"setVisible", &cAnimatedTexturedQuadRenderable::setVisible,
		"setColor", &cAnimatedTexturedQuadRenderable::setColor,
		"setWorldMatrix", &cAnimatedTexturedQuadRenderable::setWorldMatrix,
		"playAnimation", &cAnimatedTexturedQuadRenderable::playAnimationWithName
		);

	lua.new_usertype<cParticleTemplate>("ParticleTemplate",

		"setUniformFloat", [](cParticleTemplate* particle, const std::string& name, float val)
	{
		particle->addUniformFloat(name, val);
	},
		"setUniformVec2", [](cParticleTemplate* particle, const std::string& name, const Vec2& val)
	{
		particle->addUniformVec2(name, val);
	},
		"setUniformVec3", [](cParticleTemplate* particle, const std::string& name, const Vec3& val)
	{
		particle->addUniformVec3(name, val);
	},
		"setUniformVec4", [](cParticleTemplate* particle, const std::string& name, const Vec4& val)
	{
		particle->addUniformVec4(name, val);
	},

		"setUniformInt", [](cParticleTemplate* particle, const std::string& name, int val)
	{
		particle->addUniformInt(name, val);
	},
		"setUniformIntVec2", [](cParticleTemplate* particle, const std::string& name, const IntVec2& val)
	{
		particle->addUniformIntVec2(name, val);
	},
		"setUniformIntVec3", [](cParticleTemplate* particle, const std::string& name, const IntVec3& val)
	{
		particle->addUniformIntVec3(name, val);
	},
		"setUniformIntVec4", [](cParticleTemplate* particle, const std::string& name, const IntVec4& val)
	{
		particle->addUniformIntVec4(name, val);
	}
		);

	lua.new_usertype<cParticle>("Particle",
		"setLevel", &cParticle::setLevel,
		"setAlignment", &cParticle::setAlignment,
		"setVisible", &cParticle::setVisible,
		"setColor", &cParticle::setColor,
		"setWorldMatrix", &cParticle::setWorldMatrix,
		"addParticle", &cParticle::addParticle,
		"addParticleWithoutArgs", &cParticle::addParticleWithoutArgs,
		"args", &cParticle::args,
		"addLinearRandom", &cParticle::addLinearRandomizerWithName,
		"addAngularRandom", &cParticle::addAngularRandomizerWithName,
		"addUVRandom", &cParticle::addUVRandomizerWithName,
		"addVectorRandomWithName", &cParticle::addVectorRandomizerWithName,
		"setPosShiftAngular", &cParticle::setPosShiftAngular,
		"setPosShiftLinear", &cParticle::setPosShiftLinear,

		"setUniformFloat", [](cParticle* particle, const std::string& name, float val)
	{
		particle->addUniformFloat(name, val);
	},
		"setUniformVec2", [](cParticle* particle, const std::string& name, const Vec2& val)
	{
		particle->addUniformVec2(name, val);
	},
		"setUniformVec3", [](cParticle* particle, const std::string& name, const Vec3& val)
	{
		particle->addUniformVec3(name, val);
	},
		"setUniformVec4", [](cParticle* particle, const std::string& name, const Vec4& val)
	{
		particle->addUniformVec4(name, val);
	},

		"setUniformInt", [](cParticle* particle, const std::string& name, int val)
	{
		particle->addUniformInt(name, val);
	},
		"setUniformIntVec2", [](cParticle* particle, const std::string& name, const IntVec2& val)
	{
		particle->addUniformIntVec2(name, val);
	},
		"setUniformIntVec3", [](cParticle* particle, const std::string& name, const IntVec3& val)
	{
		particle->addUniformIntVec3(name, val);
	},
		"setUniformIntVec4", [](cParticle* particle, const std::string& name, const IntVec4& val)
	{
		particle->addUniformIntVec4(name, val);
	},

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
		"id", sol::readonly(&Bullet::id),

		"position", sol::property(&Bullet::getPosition, &Bullet::setPosition),
		"moveSpeed", &Bullet::moveSpeed,
		"moveAngle", &Bullet::moveAngle,
		"lifeTime", &Bullet::lifeTime,
		"startTime", &Bullet::startTime,
		"meshRotation", &Bullet::meshRotation,

		"hasCollision", &Bullet::hasCollision,

		"meshScale", &Bullet::meshScale,
		"scale", sol::readonly(&Bullet::scale),
		"setScale", [](Bullet& b, float scale) { b.scale = scale; b.updateDrawable(); },

		"moveDir", sol::readonly(&Bullet::moveDir),
		"moveVelocity", sol::readonly(&Bullet::moveVelocity),

		"radius", &Bullet::radius,
		"damage", &Bullet::damage,

		"monsterBullet", sol::readonly(&Bullet::monsterBullet),

		"penetrateCount", &Bullet::penetrateCount,
		"penetrateUsed", sol::readonly(&Bullet::penetrateUsed),

		"setScript", &Bullet::setScript,

		"isDead", sol::readonly(&Bullet::isDead),

		"data", &Bullet::data,
		"onDamageArgs", &Bullet::onDamageArgs,

		"setPosition", &Bullet::setPosition,

		"addRenderableTexture", &Bullet::addRenderableTexture,
		"addRenderableTextureWithSize", &Bullet::addRenderableTextureWithSize,
		"addRenderableTextureWithPosAndSize", &Bullet::addRenderableTextureWithPosAndSize,

		"setColor", &Bullet::setColor,

		"modifyDrawLevel", &Bullet::modifyDrawLevel,
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

	lua.set_function("getScore",
		[&]() -> int
	{
		return bloodworks->getPlayer()->getScore();
	});

	lua.set_function("addScore",
		[&](int score)
	{
		bloodworks->getPlayer()->addScore(score);
	});

	lua.set_function("setScore",
		[&](int score)
	{
		bloodworks->getPlayer()->setScore(score);
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

	lua.set_function("getUniqueId", [&]()
	{
		return bloodworks->getUniqueId();
	});

	lua.set_function("getGlobalUniqueId", [&]()
	{
		return bloodworks->getGlobalUniqueId();
	});

	lua.set_function("addLine",
		[&](const Vec2& pos0, const Vec2& pos1, float time, int color)
	{
		debugRenderer.addLine(pos0, pos1, time, color ? color : -1);
	});

	lua.set_function("addCircle",
		[&](const Vec2& pos0, float r, float time, int color)
	{
		debugRenderer.addCircle(pos0, r, time, color ? color : -1);
	});

	lua.set_function("addExplosion",
		[&](const Vec2& pos, float scale, float speed, int minDamage, int maxDamage, float startTime, bool damagePlayer)
	{
		bloodworks->addExplosion(pos, scale, speed, minDamage, maxDamage, startTime, damagePlayer);
	});

	lua.set_function("playSound",
		[&](sol::table& args)
	{
		bloodworks->playOneShotSound(args);
	});

	lua.set_function("getFreePosition",
		[&](float radius, unsigned ignoreFlags)
	{
		return bloodworks->getCollisionController()->getFreePosition(radius, ignoreFlags);
	});

	lua.set_function("hasCircleCollision",
		[&](const Vec2& pos, float radius, unsigned ignoreFlags)
	{
		return bloodworks->getCollisionController()->hasCollision(Circle(pos, radius), ignoreFlags);
	});

	lua.set_function("hasCapsuleCollision",
		[&](const Vec2& pos, const Vec2& pos2, float radius, unsigned ignoreFlags)
	{
		return bloodworks->getCollisionController()->hasCollision(Capsule(pos, pos2, radius), ignoreFlags);
	});

	lua.set_function("hasRectCollision",
		[&](const Vec2& pos, const Vec2& size, float rotation, float radius, unsigned ignoreFlags)
	{
		return bloodworks->getCollisionController()->hasCollision(Rect(pos, size, rotation, radius), ignoreFlags);
	});

	lua.set_function("getCollisionForRay",
		[&](const Vec2& pos, const Vec2& ray, float radius, unsigned ignoreFlags)
	{
		return bloodworks->getCollisionController()->getRayDistance(pos, ray, radius, ignoreFlags);
	});

	lua.new_usertype<Bonus>("Bonus",
		"name", sol::readonly(&Bonus::name),
		"scriptName", sol::readonly(&Bonus::scriptName),
		"data", &Bonus::data,
		"setActive", &Bonus::setActive,
		"isActive", &Bonus::isActive
		);

	lua.new_usertype<Perk>("Perk",
		"name", sol::readonly(&Perk::name),
		"scriptName", sol::readonly(&Perk::scriptName),

		"level", sol::readonly(&Perk::level),
		"maxLevel", sol::readonly(&Perk::maxLevel)
		);

	lua.new_usertype<Gun>("Gun",
		"id", sol::readonly(&Gun::id),
		"data", &Gun::data,
		"name", sol::readonly(&Gun::name),
		"bulletRadius", &Gun::bulletRadius,
		"bulletSpeed", &Gun::bulletSpeed,
		"scriptTable", &Gun::scriptTable,
		"bulletLifeTime", &Gun::bulletLifeTime,
		"showShootAnimation", &Gun::showShootAnimation,
		"shootParticleColor", &Gun::shootParticleColor,
		"firingSoundFadein", &Gun::gunShootSoundFadein,
		"firingSoundFadeout", &Gun::gunShootSoundFadeout,
		"firingCurVolume", &Gun::gunShootSoundCurVolume,
		"playFiringSound", &Gun::playGunShootSound,

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

	lua.set_function("convertToScreenPosition",
		[&](const Vec2& pos) -> Vec2
	{
		return (pos - bloodworks->getCameraPos()) / bloodworks->getCameraZoom() + bloodworks->getScreenDimensions().toVec() * 0.5f;
	});

	lua.set_function("getGunCount",
		[&]() -> int
	{
		return bloodworks->getGuns().size();
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
		[&](const std::string& script, const sol::table& params) -> GameObject*
	{
		return bloodworks->getMissionController()->addGameObject(script, nullptr, params);
	});

	lua.set_function("addGameObjectUsingTemplate",
		[&](const std::string& templateName, const sol::table& params) -> GameObject*
	{
		return bloodworks->getMissionController()->addGameObjectUsingTemplate(templateName, params);
	});

	lua.set_function("addCustomBullet",
		[&](const sol::table& params) -> Bullet*
	{
		return bloodworks->getBulletController()->addCustomBullet(params);
	});

	lua.set_function("getAllMonsterTypeCount",
		[&]() -> int
	{
		return bloodworks->getMonsterController()->getTemplates().size();
	});

	lua.set_function("getMonsterTypeAt",
		[&](int i) -> MonsterTemplate*
	{
		return bloodworks->getMonsterController()->getTemplates()[i];
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

	lua.set_function("loadScript",
		[&](const std::string& path)
	{
		bloodworks->loadScript(path);
	});

	lua.set_function("getClosestMonsterWithIgnoreId",
		[&](const Vec2& pos, const sol::table& ignoreIdList) -> Monster*
	{
		std::vector<int> ignoreIds;

		if (auto argIgnoreIds = ignoreIdList)
		{
			int t = 1;
			while (argIgnoreIds[t])
			{
				ignoreIds.push_back(argIgnoreIds[t].get<int>());
				t++;
			}
		}
		return bloodworks->getMonsterController()->getClosestMonsterWithIgnoreId(pos, ignoreIds);
	});

	lua.set_function("getClosestMonsterInRange",
		[&](const Vec2& pos, float range) -> Monster*
	{
		return bloodworks->getMonsterController()->getClosestMonsterInRange(pos, range);
	});

	lua.set_function("getRandomMapPosition",
		[&]() -> Vec2
	{
		return Vec2(randFloat(bloodworks->getMapLimits().getMin().x + 50, bloodworks->getMapLimits().getMax().x - 50.0f), randFloat(bloodworks->getMapLimits().getMin().y + 50.0f, bloodworks->getMapLimits().getMax().y - 50));
	});


	lua.set_function("getRandomPosition",
		[&](sol::table args) -> Vec2
	{
		return bloodworks->getMonsterController()->getRandomPos(args);
	});

	lua.set_function("getClosestMonsterInRangeWithIgnoreId",
		[&](const Vec2& pos, float range, const sol::table& ignoreIdList) -> Monster*
	{
		std::vector<int> ignoreIds;
		if (auto argIgnoreIds = ignoreIdList)
		{
			int t = 1;
			while (argIgnoreIds[t])
			{
				ignoreIds.push_back(argIgnoreIds[t].get<int>());
				t++;
			}
		}
		return bloodworks->getMonsterController()->getClosestMonsterInRangeWithIgnoreId(pos, range, ignoreIds);
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
		[&](const Vec2& pos, const Vec2& ray, float radius, sol::table& args)
	{
		return bloodworks->getMonsterController()->getClosestMonsterOnLine(pos, ray, radius, args);
	});

	lua.set_function("runForEachMonsterInRadius",
		[&](const Vec2& pos, float radius, sol::table& args, sol::function& func)
	{
		std::function<bool(Monster*)> func2 = [&func](Monster* monster)
		{
			return func(monster);
		};
		bloodworks->getMonsterController()->runForRadius(pos, radius, args, func2);
	});

	lua.set_function("runForEachMonsterOnLine",
		[&](const Vec2& pos, const Vec2& ray, float radius, sol::table& args, sol::function& func)
	{
		std::function<bool(Monster*)> func2 = [&func](Monster* monster)
		{
			return func(monster);
		};
		bloodworks->getMonsterController()->runForRay(pos, ray, radius, args, func2);
	});

	lua.set_function("spawnRandomBonus",
		[&](const Vec2& pos)
	{
		return bloodworks->getDropController()->spawnDrop(pos);
	});

	lua.set_function("spawnRandomDrop",
		[&](const Vec2& pos)
	{
		return bloodworks->getDropController()->spawnDrop(pos);
	});

	lua.set_function("spawnRandomGun",
		[&](const Vec2& pos)
	{
		return bloodworks->getDropController()->spawnGun(pos);
	});

	lua.set_function("spawnRandomBonus",
		[&](const Vec2& pos)
	{
		return bloodworks->getDropController()->spawnBonus(pos);
	});


	lua.set_function("getCameraZoom",
		[&]()
	{
		return bloodworks->getCameraZoom();
	});
	lua.set_function("getCameraPos",
		[&]()
	{
		return bloodworks->getCameraPos();
	});
	lua.set_function("getCameraAngle",
		[&]()
	{
		return bloodworks->getCameraAngle();
	});

	lua.set_function("spawnRandomGun",
		[&](const Vec2& pos)
	{
		return bloodworks->getDropController()->spawnGun(pos);
	});

	lua.set_function("getLastBonusSpawnTime",
		[&]()
	{
		return bloodworks->getDropController()->getLastSpawnTime();
	});

	lua.set_function("getGameSpeedMultiplier",
		[&]() -> BuffFloat&
	{
		return bloodworks->getMissionController()->getGameSpeedMultiplierBuff();
	});

	lua.set_function("getRandomGun",
		[&]()
	{
		return bloodworks->getGuns()[randInt(bloodworks->getGuns().size())];
	}
	);
	lua.set_function("getRandomBonus",
		[&]()
	{
		return bloodworks->getBonuses()[randInt(bloodworks->getBonuses().size())];
	}
	);
	lua.set_function("spawnGun",
		[&](const Vec2& pos, const std::string& name)
	{
		int index = 0;
		for (auto& gun : bloodworks->getGuns())
		{
			if (name == gun->getName())
			{
				bloodworks->getDropController()->spawnGun(pos, index);
				return;
			}
			index++;
		}
	});

	lua.set_function("spawnBonus",
		[&](const Vec2& pos, const std::string& name)
	{
		int index = 0;
		for (auto& bonus : bloodworks->getBonuses())
		{
			if (name == bonus->getName())
			{
				bloodworks->getDropController()->spawnBonus(pos, index);
				return;
			}
			index++;
		}
	});


	lua.new_usertype<GameObjectTemplate>("GameObjectTemplate",
		"name", sol::readonly(&GameObjectTemplate::name),
		"scriptName", sol::readonly(&GameObjectTemplate::scriptName),
		"basePath", sol::readonly(&GameObjectTemplate::basePath),
		"jsonTable", sol::readonly(&GameObjectTemplate::jsonTable)
		);


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
		"scriptArgs", sol::readonly(&MonsterTemplate::scriptArgs),
		"basePath", sol::readonly(&MonsterTemplate::basePath)
	);

#ifdef SHOW_TIMINGS
	lua.set_function("createSumProfiler",
		[&](const std::string& name) -> cAccumulatedTimeProfiler&
	{
		return Coral::createAccumulatedTimeProfile(name.c_str());
	});

	lua.set_function("createProfiler",
		[&](const std::string& name) -> cTimeProfiler
	{
		return cTimeProfiler(name.c_str());
	});
	lua.new_usertype<cAccumulatedTimeProfiler>("SumProfiler",
		"start", &cAccumulatedTimeProfiler::start,
		"stop", &cAccumulatedTimeProfiler::stop
		);

	lua.new_usertype<cTimeProfiler>("Profiler",
		"start", &cTimeProfiler::start,
		"stop", &cTimeProfiler::stop
		);
#endif

	lua.new_usertype<Monster>("Monster",
		"name", &Monster::name,
		"id", sol::readonly(&Monster::id),

		"debugVal", &Monster::debugVal,

		"scriptTable", &Monster::scriptTable,

		"animationSpeed", &Monster::animationSpeed,

		"setVisible", &Monster::setVisible,
		"isVisible", &Monster::isVisible,

		"position", sol::property(&Monster::getPosition, &Monster::setPosition),
		"moveSpeed", &Monster::moveSpeed,
		"moveAngle", &Monster::moveAngle,
		"spriteAngle", &Monster::spriteAngle,
		"moveVelocity", sol::readonly(&Monster::moveVelocity),
		"moveDir", sol::readonly(&Monster::moveDir),

		"getPathPos", &Monster::getPathPos,

		"textureShift", &Monster::textureShift,
		"textureSize", &Monster::textureSize,

		"hitPoint", &Monster::hitPoint,

		"collisionRadius", &Monster::collisionRadius,
		"bulletRadius", &Monster::bulletRadius,

		"data", &Monster::data,
		"experienceMultiplier", &Monster::experienceMultiplier,
		"scoreMultiplier", &Monster::scoreMultiplier,

		"scale", sol::readonly(&Monster::scale),

		"moveSpeedMultiplier", &Monster::moveSpeedMultiplier,
		"colorMultiplier", &Monster::colorMultiplier,

		"isDead", sol::readonly(&Monster::isDead),
		"removeOnDead", &Monster::removeOnDead,
		"hasBlood", &Monster::hasBlood,
		"hasGibs", &Monster::hasGibs,

		"playAnimation", &Monster::playAnimation,
		"addTimer", &Monster::addTimer,
		"setScale", &Monster::setScale,

		"knockbackResistance", &Monster::knockbackResistance,
		"addKnockback", &Monster::addKnockback,

		"dropChance", &Monster::dropChance,
		"hasCollision", &Monster::hasCollision,

		"canGetOneShooted", &Monster::canGetOneShooted,

		"killSelf", [&](Monster* monster)
	{
		monster->killSelf(Vec2::zero());
	},
		"killSelfWithDir", &Monster::killSelf,

		"doDamage", &Monster::doDamage,
		"doDamageWithArgs", &Monster::doDamageWithArgs,
		"addIgnoreId", &Monster::addIgnoreId,
		"hasIgnoreId", &Monster::hasIgnoreId,
		"copyIgnoreId", &Monster::copyIgnoreId,

		"addParticleSpawner", &Monster::addParticleSpawner,

		"spawnParticle", &Monster::spawnParticle,
		"spawnParticleShifted", &Monster::spawnParticleShifted,

		"spawnParticleWithoutArgs", &Monster::spawnParticleWithoutArgs,
		"spawnParticleShiftedWithoutArgs", &Monster::spawnParticleShiftedWithoutArgs,

		"modifyDrawLevel", &Monster::modifyDrawLevel,
		"setDrawLevel", &Monster::setDrawLevel,
		"monsterTemplate", sol::readonly(&Monster::monsterTemplate)
		);

	lua.new_usertype<GameObject>("GameObject",
		"id", sol::readonly(&GameObject::id),

		"toBeRemoved", &GameObject::toBeRemoved,
		"script", &GameObject::script,
		"data", &GameObject::data,
		"addText", &GameObject::addText,
		"addTexture", &GameObject::addTexture,
		"addParticle", &GameObject::addParticle,
		"addAnimation", &GameObject::addAnimation,

		"removeAnimation", &GameObject::removeAnimation,
		"removeParticle", &GameObject::removeParticle,
		"removeText", &GameObject::removeText,
		"removeTexture", &GameObject::removeTexture,


		"setPosition", &GameObject::setPosition,
		"setRotation", &GameObject::setRotation,
		"setScale", &GameObject::setScale,

		"getPosition", &GameObject::getPosition,
		"getRotation", &GameObject::getRotation,
		"getScale", &GameObject::getScale,

		"addCircleCollider", &GameObject::addCircleCollider,
		"addRectCollider", &GameObject::addRectCollider,
		"addCapsuleCollider", &GameObject::addCapsuleCollider,

		"setLevel", &GameObject::setLevel,
		"setAlignment", &GameObject::setAlignment

		);

	lua.new_usertype<Player>("Player",
		"position", sol::readonly(&Player::pos),
		"moveSpeed", sol::readonly(&Player::moveSpeed),
		"crosshairPos", sol::readonly(&Player::crosshairPos),
		"crosshairDistance", sol::readonly(&Player::crosshairDistance),
		"gunPos", sol::readonly(&Player::gunPos),
		"aimDir", sol::readonly(&Player::aimDir),
		"aimAngle", sol::readonly(&Player::aimAngle),
		"moveAngle", sol::readonly(&Player::moveAngle),
		"moveDir", sol::readonly(&Player::moveDir),
		"visible", sol::readonly(&Player::visible),
		"isDead", sol::readonly(&Player::isDead),
		"colorMultiplier", &Player::colorMultiplier,
		"moveVelocity", sol::readonly(&Player::moveVelocity),
		"hitPoints", sol::readonly(&Player::hitPoints),
		"maxHitPoints", sol::readonly(&Player::maxHitPoints),
		"maxSpeed", &Player::maxSpeed,
		"maxRotateSpeed", &Player::maxRotateSpeed,
		"doDamage", &Player::doDamage,
		"doDamageWithArgs", &Player::doDamageWithArgs,
		"doHeal", &Player::doHeal,
		"gainExperience", &Player::gainExperience,
		"bulletSpeedMultiplier", &Player::bulletSpeedMultiplier,
		"shootSpeedMultiplier", &Player::shootSpeedMultiplier,
		"monsterExperienceMultiplier", &Player::monsterExperienceMultiplier,
		"damageMultiplier", &Player::damageMultiplier,
		"reloadSpeedMultiplier", &Player::reloadSpeedMultiplier,
		"globalMonsterSpeedMultiplier", &Player::globalMonsterSpeedMultiplier,
		"clipCountMultiplier", &Player::clipCountMultiplier,
		"gunSpreadMultiplier", &Player::gunSpreadMultiplier,
		"accelerationMultiplier", &Player::accelerationMultiplier,
		"setGun", &Player::setGun,
		"level", sol::readonly(&Player::level),
		"gun", sol::readonly(&Player::gun),
		"experience", sol::readonly(&Player::experience),
		"experienceForNextLevel", sol::readonly(&Player::experienceForNextLevel),
		"playShootAnimation", &Player::playShootAnimation,
		"getMaxHitpoints", &Player::getMaxHitpoints,
		"getHitpoints", &Player::getHitpoints,
		"setMaxHitpoints", &Player::setMaxHitpoints,
		"setHitpoints", &Player::setHitpoints,
		"addKnockback", &Player::addKnockback,
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
		"getRemainingTime", &BuffFloat::BuffInfo::getRemainingTime,

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

		"removeBuff", &BuffFloat::removeBuff,
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
		"getBuffRemainingTime", &BuffFloat::getBuffRemainingTime,

		"setUseRealtime", &BuffFloat::setUseRealtime,

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
		"removeBuff", &BuffVec4::removeBuff,
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
		"getBuffRemainingTime", &BuffVec4::getBuffRemainingTime,

		"setUseRealtime", &BuffVec4::setUseRealtime,

		"getBuffInfo", &BuffVec4::getBuffInfo,
		"hasBuffInfo", &BuffVec4::hasBuffInfo,

		"getBaseValue", &BuffVec4::getBaseValue,
		"getBuffedValue", &BuffVec4::getBuffedValue

		);


	lua.set_function("addCircleCollider",
		[&](const Vec2& pos, float radius)
	{
		return bloodworks->getCollisionController()->addCollider(Circle(pos, radius), nullptr);
	});
	lua.set_function("addCapsuleCollider",
		[&](const Vec2& pos0, const Vec2& pos1, float radius)
	{
		return bloodworks->getCollisionController()->addCollider(Capsule(pos0, pos1, radius), nullptr);
	});
	lua.set_function("addRectCollider",
		[&](const Vec2& pos, const Vec2& size, float rotation, float radius)
	{
		return bloodworks->getCollisionController()->addCollider(Rect(pos, size, rotation, radius), nullptr);
	});

	lua.set_function("relocateCircleCollider",
		[&](int index, const Vec2& pos, float radius)
	{
		return bloodworks->getCollisionController()->relocateCollider(index, Circle(pos, radius));
	});
	lua.set_function("relocateCapsuleCollider",
		[&](int index, const Vec2& pos0, const Vec2& pos1, float radius)
	{
		return bloodworks->getCollisionController()->relocateCollider(index, Capsule(pos0, pos1, radius));
	});
	lua.set_function("relocateRectCollider",
		[&](int index, const Vec2& pos, const Vec2& size, float rotation, float radius)
	{
		return bloodworks->getCollisionController()->relocateCollider(index, Rect(pos, size, rotation, radius));
	});

	lua.set_function("removeCollider",
		[&](int index)
	{
		bloodworks->getCollisionController()->removeCollider(index);
	});

	lua.set_function("setColliderFlag",
		[&](int index, int flag, bool isSet)
	{
		bloodworks->getCollisionController()->setFlag(index, flag, true);
	});
	lua.set_function("setColliderFlags",
		[&](int index, int flags)
	{
		bloodworks->getCollisionController()->setFlags(index, flags);
	});
	lua.set_function("clearColliderFlag",
		[&](int index, int flag, bool isSet)
	{
		bloodworks->getCollisionController()->setFlag(index, flag, false);
	});
	lua.set_function("hasColliderFlag",
		[&](int index, int flag, bool isSet)
	{
		return bloodworks->getCollisionController()->hasFlag(index, flag);
	});
	lua.set_function("getColliderFlags",
		[&](int index, int flag, bool isSet)
	{
		return bloodworks->getCollisionController()->getFlags(index);
	});
}


BloodworksLuaWorld::~BloodworksLuaWorld()
{
	clear();
	lua["deleteAll"]();
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
