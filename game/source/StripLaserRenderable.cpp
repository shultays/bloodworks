#include "StripLaserRenderable.h"
#include "bloodworks.h"
#include "cGlobals.h"
#include "cParticle.h"

StripLaserRenderable::StripLaserRenderable(Bloodworks* bloodworks, std::string particleTemplate, int level )
{
	first = true;
	params = lua.create_table();
	this->bloodworks = bloodworks;
	this->particleTemplate = particleTemplate;
	this->level = level;
	particle = nullptr;
	shootSpeedOverride.setZero();
	posOverride.setZero();
	stop();
}

void StripLaserRenderable::tick(const Vec2& p, const Vec2& s)
{
	if (shooting)
	{
		Vec2 speed = s;
		if (shootSpeedOverride.lengthSquared() > 0.001f)
		{
			speed = shootSpeedOverride;
		}

		Vec2 pos = p;
		if (posOverride.lengthSquared() > 0.001f)
		{
			pos = posOverride;
		}
		if (particle == nullptr)
		{
			sol::table args = lua.create_table();
			particle = new cParticle(bloodworks, bloodworks->getParticleTemplate(particleTemplate), args);
			particle->setWorldMatrix(Mat3::identity());
			bloodworks->addRenderable(particle, level);
		}
		shootTime -= timer.getDt();
		bool replaceLast = true;
		static bool dontAdd = false;

		if (input.isKeyPressed(key_t))
		{
			dontAdd = !dontAdd;
		}
		if (shootTime < 0.0f && dontAdd == false)
		{
			shootTime = 0.01f;
			replaceLast = false;
		}

		float time = timer.getTime() - particle->getStartTime();

		if (first)
		{
			prevPos = pos;
			prevSpeed = speed;
			prevTime = time;
		}

		Vec2 s = first && replaceLast == false ? speed : Vec2::zero();
		if (replaceLast == false && first == false)
		{
			params["speed"] = prevSpeed;
			particle->addParticleInternal(prevPos, &params, nullptr, true);
			s = prevSpeed;
		}
		params["speed"] = s;
		params["prevpos"] = prevPos;
		params["prevspeed"] = prevSpeed;
		params["prevtime"] = prevTime;

		first = false;
		if (input.isKeyPressed(key_r) || dontAdd == false)
		{
			//out << "------------\n";
			//out << "pos " << pos.x << " " << pos.y << "\n";
			//
			//Vec2 t;
			//t = params["speed"].get<Vec2>();
			//out << "speed " << t.x << " " << t.y << "\n";
			//t = params["prevpos"].get<Vec2>();
			//out << "prevpos " << t.x << " " << t.y << "\n";
			//t = params["prevspeed"].get<Vec2>();
			//out << "prevspeed " << t.x << " " << t.y << "\n";
			//out << "prevtime " << params["prevtime"].get<float>() << "\n";

			particle->addParticleInternal(pos, &params, nullptr, replaceLast);
		}

		if (!replaceLast)
		{
			prevPos = pos;
			prevSpeed = speed;
			prevTime = time;
		}
	}
	else
	{
		stop();
	}
}

void StripLaserRenderable::stop()
{
	first = true;
	shootTime = 0.0f;
	shooting = false;
	if (particle)
	{
		params["speed"] = prevSpeed;
		particle->addParticleInternal(prevPos, &params, nullptr, true);
		bloodworks->addOrphanParticle(particle);
		particle = nullptr;
	}
}

void StripLaserRenderable::setShooting(bool shooting)
{
	this->shooting = shooting;
}

void StripLaserRenderable::setSpeed(const Vec2& speed)
{
	this->shootSpeedOverride = speed;
}
void StripLaserRenderable::setPos(const Vec2& pos)
{
	this->posOverride = pos;
}

void StripLaserRenderable::clear()
{
	SAFE_DELETE(particle);
	stop();
}
