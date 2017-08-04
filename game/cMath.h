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

	static float distSegmentToSegment(const Vec2& p0, const Vec2& s1, const Vec2& p2, const Vec2& s2)
	{
		float s, t;
		s = (-s1.y * (p0.x - p2.x) + s1.x * (p0.y - p2.y)) / (-s2.x * s1.y + s1.x * s2.y);
		t = (s2.x * (p0.y - p2.y) - s2.y * (p0.x - p2.x)) / (-s2.x * s1.y + s1.x * s2.y);

		if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
		{
			return t;
		}

		return -1.0f;
	}

	static float rayCapsuleIntersection(const Vec2& pos, const Vec2& ray, const Vec2& capsuleA, const Vec2& capsuleB, float radius)
	{
		Vec2 toCapsule = capsuleB - capsuleA;

		Vec2 dir = toCapsule;
		float length = dir.normalize();
		dir = dir.sideVec();

		float r = FLT_MAX;
		float s[4];
		float rayLength = ray.length();
		s[0] = distSegmentToSegment(pos, ray, capsuleA + dir * radius, toCapsule) * rayLength;
		s[1] = distSegmentToSegment(pos, ray, capsuleA - dir * radius, toCapsule) * rayLength;
		s[2] = rayCircleIntersection(pos, ray, capsuleA, radius);
		s[3] = rayCircleIntersection(pos, ray, capsuleB, radius);
		for (int i = 0; i < 4; i++)
		{
			if (s[i] >= 0.0f && s[i] < r)
			{
				r = s[i];
			}
		}

		if (r >= 0.0f && r <= FLT_MAX - 5.0f)
		{
			return r;
		}
		return -1.0f;
	}
};