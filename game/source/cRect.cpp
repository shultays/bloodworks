#include "cRect.h"
#include "cCapsule.h"
#include "cCircle.h"
#include "cMath.h"

bool Rect::doesIntersect(const Rect& r) const
{
	AARect rect = getAABB();
	AARect rect2 = r.getAABB();
	return rect.doesIntersect(rect2);
}

bool Rect::doesIntersect(const Capsule& capsule) const
{
	return capsule.doesIntersect(*this);
}

bool Rect::doesIntersect(const Circle& circle) const
{
	Vec2 diff = circle.getOrigin() - center;
	Vec2 transformed = diff * rotationMat;
	float radius = circle.getRadius();
	return transformed.x > -halfSize.x - radius
		&& transformed.y > -halfSize.y - radius
		&& transformed.x < +halfSize.x + radius
		&& transformed.y < +halfSize.y + radius;
}

Vec2 Rect::getSolver(const Circle& circle) const
{
	Mat2 inverted = rotationMat.inverse();
	Vec2 diff = circle.getOrigin() - center;
	Vec2 transformed = diff * rotationMat;
	float r = circle.getRadius() + radius;

	Vec2 closestPoint;

	int inside = 0;
	if (transformed.x < -halfSize.x)
	{
		closestPoint.x = -halfSize.x;
	}
	else if (transformed.x > halfSize.x)
	{
		closestPoint.x = halfSize.x;
	}
	else
	{
		closestPoint.x = transformed.x;
		inside++;
	}

	if (transformed.y < -halfSize.y)
	{
		closestPoint.y = -halfSize.y;
	}
	else if (transformed.y > halfSize.y)
	{
		closestPoint.y = halfSize.y;
	}
	else
	{
		closestPoint.y = transformed.y;
		inside++;
	}

	if (inside == 2)
	{
		float diffX = halfSize.x - abs(transformed.x);
		float diffY = halfSize.y - abs(transformed.y);

		float d = r * r - diffX * diffX - diffY * diffY;
		if (d > 0.0f)
		{
			Vec2 toCircle(transformed.x > 0.0 ? diffX : -diffX, transformed.y > 0.0 ? diffY : -diffY);
			float t = toCircle.normalize();
			return toCircle * (t + sqrtf(d)) * inverted;
		}
		else if (diffX < diffY)
		{
			if (transformed.x > 0.0f)
			{
				return Vec2(diffX + r, 0.0f) * inverted;
			}
			else
			{
				return -Vec2(diffX + r, 0.0f) * inverted;
			}
		}
		else
		{
			if (transformed.y > 0.0f)
			{
				return Vec2(0.0f, diffY + r) * inverted;
			}
			else
			{
				return -Vec2(0.0f, diffY + r) * inverted;
			}
		}
	}
	else
	{
		Vec2 toCircle = transformed - closestPoint;
		float l = toCircle.normalize();
		if (l < r)
		{
			return toCircle * (r - l) * inverted;
		}
	}


	return Vec2::zero();
}



float Rect::doesHit(const Vec2& begin, const Vec2& ray, float radius)
{
	float r = this->radius + radius;
	Vec2 corners[4];

	Mat2 inverse = Mat2::rotation(angle);

	float ret = FLT_MAX;

	if (r < 0.0001f)
	{
		corners[0] = center + Vec2(halfSize.x, halfSize.y) * inverse;
		corners[1] = center + Vec2(halfSize.x, -halfSize.y) * inverse;
		corners[2] = center + Vec2(-halfSize.x, -halfSize.y) * inverse;
		corners[3] = center + Vec2(-halfSize.x, halfSize.y) * inverse;
	
		float ss[4];
		float rayLength = ray.length();
		ss[0] = rayLength * cMath::distSegmentToSegment(begin, ray, corners[0], corners[1] - corners[0]);
		ss[1] = rayLength * cMath::distSegmentToSegment(begin, ray, corners[1], corners[2] - corners[1]);
		ss[2] = rayLength * cMath::distSegmentToSegment(begin, ray, corners[2], corners[3] - corners[2]);
		ss[3] = rayLength * cMath::distSegmentToSegment(begin, ray, corners[3], corners[0] - corners[3]);

		for (int i = 0; i < 4; i++)
		{
			if (ss[i] >= 0.0f && ss[i] < ret)
			{
				ret = ss[i];
			}
		}
	}
	else
	{
		Vec2 v(r + 0.1f, 0.0f);
		Vec2 v2(0.0, -r - 0.1f);
		v = v * inverse;
		v2 = v2 * inverse;

		corners[0] = center + Vec2(halfSize.x + r, halfSize.y + r) * inverse;
		corners[1] = center + Vec2(halfSize.x + r, -halfSize.y - r) * inverse;
		corners[2] = center + Vec2(-halfSize.x - r, -halfSize.y - r) * inverse;
		corners[3] = center + Vec2(-halfSize.x - r, halfSize.y + r) * inverse;

		float ss[4];
		float rayLength = ray.length();
		ss[0] = rayLength * cMath::distSegmentToSegment(begin, ray, corners[0] + v, corners[1] - corners[0]);
		ss[1] = rayLength * cMath::distSegmentToSegment(begin, ray, corners[1] + v2, corners[2] - corners[1]);
		ss[2] = rayLength * cMath::distSegmentToSegment(begin, ray, corners[2] - v, corners[3] - corners[2]);
		ss[3] = rayLength * cMath::distSegmentToSegment(begin, ray, corners[3] - v2, corners[0] - corners[3]);

		for (int i = 0; i < 4; i++)
		{
			float s = cMath::rayCircleIntersection(begin, ray, corners[i], r);

			if (s >= 0.0f && s < ret)
			{
				ret = s;
			}
			if (ss[i] >= 0.0f && ss[i] < ret)
			{
				ret = ss[i];
			}
		}
	}

	if (ret >= 0.0f && ret <= FLT_MAX - 5.0f)
	{
		return ret;
	}
	return -1.0f;
}
