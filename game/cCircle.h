#pragma once

#include "cTools.h"
#include "cGlobals.h"

class Rect;
class Capsule;

class Circle
{
	Vec2 origin;
	float radius;

public:
	Circle(){}
	Circle(const Vec2& origin, float radius)
	{
		this->origin = origin;
		this->radius = radius;
	}

	AARect getAABB() const
	{
		return AARect(origin - radius, origin + radius);
	}

	const Vec2& getOrigin() const
	{
		return origin;
	}

	float getRadius() const
	{
		return radius;
	}

	bool doesIntersect(const Vec2& point) const
	{
		return point.distanceSquared(origin) < radius * radius;
	}

	bool doesIntersect(const Circle& circle) const;

	bool doesIntersect(const Capsule& capsule) const;

	bool doesIntersect(const Rect& rect) const;

	void drawDebug(int color = 0xFFFFFFFF, float time = 0.0f)
	{
		debugRenderer.addCircle(origin, radius, time, color);
	}

	float doesHit(const Vec2& begin, const Vec2& ray, float radius = 0.0f);

	Vec2 getSolver(const Circle& circle) const
	{

		float r = radius + circle.radius;
		float t = r * r - circle.origin.distanceSquared(origin);
		if (t > 0.0f)
		{
			Vec2 toCircle = circle.origin - origin;
			float d = toCircle.normalize();
			return toCircle * (r - d);
		}
		return Vec2::zero();
	}
};