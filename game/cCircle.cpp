#include "cCircle.h"
#include "cCapsule.h"
#include "cRect.h"
#include "cMath.h"

bool Circle::doesIntersect(const Rect& rect) const
{
	return rect.doesIntersect(*this);
}

bool Circle::doesIntersect(const Capsule& capsule) const
{
	return capsule.doesIntersect(*this);
}

bool Circle::doesIntersect(const Circle& circle) const
{
	float r = radius + circle.radius;
	return circle.origin.distanceSquared(origin) < r * r;
}

float Circle::doesHit(const Vec2& begin, const Vec2& ray, float radius)
{
	return cMath::rayCircleIntersection(begin, ray, origin, radius + this->radius);
}
