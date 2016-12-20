#pragma once


#include "cVec.h"
class cMath
{
public:
	static float capsuleCircleIntersection(const Vec2& rayBegin, const Vec2& ray, float capsuleRadius, const Vec2& circleMid, float radius)
	{
		return rayCircleIntersection(rayBegin, ray, circleMid, radius + capsuleRadius);
	}

	static float rayCircleIntersection(const Vec2& rayBegin, const Vec2& ray, const Vec2& circleMid, float radius)
	{
		Vec2 d = ray;
		Vec2 f = rayBegin - circleMid;
		float a = d.lengthSquared();
		float b = 2 * f.dot(d);
		float c = f.dot(f) - radius * radius;

		float discriminant = b*b - 4 * a*c;
		if (discriminant < 0.0f)
		{
			return -1.0f;
		}
		else
		{
			discriminant = sqrt(discriminant);

			float t1 = (-b - discriminant) / (2 * a);
			float t2 = (-b + discriminant) / (2 * a);

			if (t1 >= 0 && t1 <= 1)
			{
				return t1 * ray.length();
			}

			if (t2 >= 0 && t2 <= 1)
			{
				return t2 * ray.length();
			}

			return -1.0f;
		}
	}
};