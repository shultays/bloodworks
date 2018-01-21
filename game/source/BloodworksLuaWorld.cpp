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
#include "StripLaserRenderable.h"
#include "BloodworksSteam.h"
#include "BloodRenderable.h"
#include "ExplosionController.h"

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

		"tostr", [](const Vec2& a) -> std::string {
		std::stringstream s;
		s << a.x << " " << a.y;
		return s.str();
	},

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

		"setTexture", [](cTexturedQuadRenderable* particle, const std::string& name)
	{
		particle->setTexture(name);
	},

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



	lua.new_usertype<cPersistent>("Config",
		"getBool", [](cPersistent* config, const std::string& name, bool val, const std::string& comment)
	{
		return config->getBool(name, val, comment.c_str());
	},
		"getFloat", [](cPersistent* config, const std::string& name, float val, const std::string& comment)
	{
		return config->getFloat(name, val, comment.c_str());
	},
		"getInt", [](cPersistent* config, const std::string& name, int val, const std::string& comment)
	{
		return config->getInt(name, val, comment.c_str());
	},
		"getVec2", [](cPersistent* config, const std::string& name, const Vec2& val, const std::string& comment)
	{
		return config->getVec2(name, val, comment.c_str());
	},
		"getIntVec2", [](cPersistent* config, const std::string& name, const IntVec2& val, const std::string& comment)
	{
		return config->getIntVec2(name, val, comment.c_str());
	},
		"getString", [](cPersistent* config, const std::string& name, const std::string& val, const std::string& comment)
	{
		return config->getString(name, val, comment.c_str());
	},

		"setBool", [](cPersistent* config, const std::string& name, bool val)
	{
		config->set(name, val);
	},
		"setFloat", [](cPersistent* config, const std::string& name, float val)
	{
		config->set(name, val);
	},
		"setInt", [](cPersistent* config, const std::string& name, int val)
	{
		config->set(name, val);
	},
		"setIntMax", [](cPersistent* config, const std::string& name, int val)
	{
		if (config->getInt(name) < val)
		{
			config->set(name, val);
		}
	},
		"setVec2", [](cPersistent* config, const std::string& name, const Vec2& val)
	{
		config->set(name, val);
	},
		"setIntVec2", [](cPersistent* config, const std::string& name, const IntVec2& val)
	{
		config->set(name, val);
	},
		"setString", [](cPersistent* config, const std::string& name, const std::string& val)
	{
		config->set(name, val);
	}

	);

	lua.new_usertype<MissionMod>("MissionMod",
		"data", &MissionMod::data,
		"config", &MissionMod::persistent
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
		"addParticleWithoutArgs", [](cParticle* particle, const Vec2& name)
	{
		particle->addParticleWithoutArgs(name);
	},
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

	init2();
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
	LastEntrySet S("BloodworksLuaWorld::tick");
	if (bloodworks->isFirstTick())
	{
		out << "BloodworksLuaWorld::tick\n";
	}
	buffController->tick();
}
