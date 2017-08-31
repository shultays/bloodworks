#include "cCapsule.h"
#include "cRect.h"
#include "cCircle.h"
#include "cMath.h"

bool Capsule::doesIntersect(const Circle& circle) const
{
	float l = (circle.getOrigin() - pos0).dot(dir);
	clamp(l, 0.0f, len);
	float r = radius + circle.getRadius();
	return circle.getOrigin().distanceSquared(pos0 + dir * l) < r * r;
}

bool Capsule::doesIntersect(const Capsule& capsule) const
{
	AARect rect = getAABB();
	AARect rect2 = capsule.getAABB();
	return rect.doesIntersect(rect2);
}

bool Capsule::doesIntersect(const Rect& r) const
{
	AARect rect = getAABB();
	AARect rect2 = r.getAABB();
	return rect.doesIntersect(rect2);
}

Vec2 Capsule::getSolver(const Circle& circle) const
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


float Capsule::doesHit(const Vec2& begin, const Vec2& ray, float radius)
{
	return cMath::rayCapsuleIntersection(begin, ray, pos0, pos1, radius + this->radius);
}
