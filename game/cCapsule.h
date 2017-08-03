#pragma once

#include "cTools.h"
#include "cCircle.h"

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


	bool doesIntersect(const Vec2& point) const
	{
		float l = (point - pos0).dot(dir);
		clamp(l, 0.0f, len);
		return point.distanceSquared(pos0 + dir * l) < radius * radius;

	}

	bool doesIntersect(const Circle& circle) const
	{
		float l = (circle.getOrigin() - pos0).dot(dir);
		clamp(l, 0.0f, len);
		float r = radius + circle.getRadius();
		return circle.getOrigin().distanceSquared(pos0 + dir * l) < r * r;
	}


	void drawDebug(int color = 0xFFFFFFFF, float time = 0.0f)
	{
		debugRenderer.addCircle(pos0, radius, time, color);
		debugRenderer.addCircle(pos1, radius, time, color);
		debugRenderer.addLine(pos0 + dir.sideVec() * radius, pos1 + dir.sideVec() * radius, time, color);
		debugRenderer.addLine(pos0 - dir.sideVec() * radius, pos1 - dir.sideVec() * radius, time, color);
	}

	Vec2 getSolver(const Circle& circle) const
	{
		Vec2 toCircle = circle.getOrigin() - pos0;
		float l = toCircle.dot(dir);
		float r = circle.getRadius() + radius;
		if (l < 0.0f)
		{
			float d = circle.getOrigin().distanceSquared(pos0);
			if (d < r * r)
			{
				Vec2 toCircle = circle.getOrigin() - pos0;
				d = toCircle.normalize();
				return toCircle * (r - d);
			}
		}
		else if (l > len)
		{
			float d = circle.getOrigin().distanceSquared(pos1);
			if (d < r * r)
			{
				Vec2 toCircle = circle.getOrigin() - pos1;
				d = toCircle.normalize();
				return toCircle * (r - d);
			}
		}
		else
		{
			float d = circle.getOrigin().distanceSquared(pos0 + dir * l);
			if (d < r * r)
			{
				d = sqrt(d) - r;
				if (dir.sideVec().dot(toCircle) < 0.0f)
				{
					return dir.sideVec() * d;
				}
				else
				{
					return -dir.sideVec() * d;
				}
			}
		}
		return Vec2::zero();
	}
};