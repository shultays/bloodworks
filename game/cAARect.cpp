#include "cAARect.h"
#include "cTools.h"

AARect AARect::transform(const Mat3& mat) const
{
	return AARect((mat * Vec3(minPoint, 1.0f)).vec2, (mat * Vec3(maxPoint, 1.0f)).vec2);
}

void AARect::clampPos(Vec2& position) const
{
	position.x = max(position.x, minPoint.x);
	position.y = max(position.y, minPoint.y);
	position.x = min(position.x, maxPoint.x);
	position.y = min(position.y, maxPoint.y);
}

Vec2 AARect::getRandomPos() const
{
	Vec2 p;
	p.x = randFloat(minPoint.x, maxPoint.x);
	p.y = randFloat(minPoint.y, maxPoint.y);
	return p;
}

