#pragma once

#include "cTools.h"

class Circle
{
	Vec2 origin;
	float radius;

public:
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
		return point.lengthSquared(origin) < radius * radius;
	}

	bool doesIntersect(const Circle& circle) const
	{
		float r = radius + circle.radius;
		return circle.origin.lengthSquared(origin) < r * r;
	}


};