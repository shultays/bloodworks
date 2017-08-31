#pragma once

#include "cTools.h"
#include "cGlobals.h"

class Circle;
class Rect;

class Capsule
{
	Vec2 pos0;
	Vec2 pos1;
	float radius;
	float len;
	Vec2 dir;

public:
	Capsule(){}
	Capsule(const Vec2& pos0, const Vec2& pos1, float radius)
	{
		this->pos0 = pos0;
		this->pos1 = pos1;
		this->radius = radius;
		dir = pos1 - pos0;
		len = dir.normalize();
	}

	AARect getAABB() const
	{
		Vec2 minPoint;
		minPoint.x = min(pos0.x, pos1.x);
		minPoint.y = min(pos0.y, pos1.y);
		Vec2 maxPoint;
		maxPoint.x = max(pos0.x, pos1.x);
		maxPoint.y = max(pos0.y, pos1.y);
		AARect rect(minPoint, maxPoint);
		rect.addThreshold(radius);
		return rect;
	}

	const Vec2& getPos0() const
	{
		return pos0;
	}

	const Vec2& getPos1() const
	{
		return pos1;
	}

	float getRadius() const
	{
		return radius;
	}

	float doesHit(const Vec2& begin, const Vec2& ray, float radius);

	bool doesIntersect(const Vec2& point) const
	{
		float l = (point - pos0).dot(dir);
		clamp(l, 0.0f, len);
		return point.distanceSquared(pos0 + dir * l) < radius * radius;

	}

	bool doesIntersect(const Circle& circle) const;
	bool doesIntersect(const Capsule& capsule) const;
	bool doesIntersect(const Rect& r) const;

	void drawDebug(int color = 0xFFFFFFFF, float time = 0.0f)
	{
		debugRenderer.addCircle(pos0, radius, time, color);
		debugRenderer.addCircle(pos1, radius, time, color);
		debugRenderer.addLine(pos0 + dir.sideVec() * radius, pos1 + dir.sideVec() * radius, time, color);
		debugRenderer.addLine(pos0 - dir.sideVec() * radius, pos1 - dir.sideVec() * radius, time, color);
	}

	Vec2 getSolver(const Circle& circle) const;
};