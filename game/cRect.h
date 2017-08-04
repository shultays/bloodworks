#pragma once

#include "cTools.h"
#include "cGlobals.h"

class Circle;
class Capsule;

class Rect
{
	Vec2 center;
	Vec2 halfSize;

	float angle;
	Mat2 rotationMat;
	float radius;

public:
	Rect(){}
	Rect(const Vec2& center, const Vec2& halfSize, float angle, float radius = 0.0f)
	{
		this->center = center;
		this->halfSize = halfSize;
		this->angle = angle;
		this->radius = radius;
		rotationMat = Mat2::rotation(-angle);
	}

	bool doesIntersect(const Vec2& point) const
	{
		Vec2 diff = point - center;
		Vec2 transformed = diff * rotationMat;

		return transformed.x > -halfSize.x - radius
			&& transformed.y > -halfSize.y - radius
			&& transformed.x < +halfSize.x + radius
			&& transformed.y < +halfSize.y + radius;
	}

	bool doesIntersect(const Circle& circle) const;
	bool doesIntersect(const Capsule& capsule) const;
	bool doesIntersect(const Rect& r) const;
	float doesHit(const Vec2& begin, const Vec2& ray, float radius);
	Vec2 getSolver(const Circle& circle) const;

	void drawDebug(int color = 0xFFFFFFFF, float time = 0.0f) const
	{
		Vec2 corners[4];

		Mat2 inverse = Mat2::rotation(angle);

		if (radius < 0.0001f)
		{
			corners[0] = center + Vec2(halfSize.x, halfSize.y) * inverse;
			corners[1] = center + Vec2(halfSize.x, -halfSize.y) * inverse;
			corners[2] = center + Vec2(-halfSize.x, -halfSize.y) * inverse;
			corners[3] = center + Vec2(-halfSize.x, halfSize.y) * inverse;

			debugRenderer.addLine(corners[0], corners[1], time, color);
			debugRenderer.addLine(corners[1], corners[2], time, color);
			debugRenderer.addLine(corners[2], corners[3], time, color);
			debugRenderer.addLine(corners[3], corners[0], time, color);

		}
		else
		{
			Vec2 v(radius, 0.0f);
			Vec2 v2(0.0, -radius);
			v = v * inverse;
			v2 = v2 * inverse;

			corners[0] = center + Vec2(halfSize.x + radius, halfSize.y + radius) * inverse;
			corners[1] = center + Vec2(halfSize.x + radius, -halfSize.y - radius) * inverse;
			corners[2] = center + Vec2(-halfSize.x - radius, -halfSize.y - radius) * inverse;
			corners[3] = center + Vec2(-halfSize.x - radius, halfSize.y + radius) * inverse;

			debugRenderer.addCircle(corners[0], radius, time, color);
			debugRenderer.addCircle(corners[1], radius, time, color);
			debugRenderer.addCircle(corners[2], radius, time, color);
			debugRenderer.addCircle(corners[3], radius, time, color);

			debugRenderer.addLine(corners[0] + v, corners[1] + v, time, color);
			debugRenderer.addLine(corners[1] + v2, corners[2] + v2, time, color);
			debugRenderer.addLine(corners[2] - v, corners[3] - v, time, color);
			debugRenderer.addLine(corners[3] - v2, corners[0] - v2, time, color);
		}
	}

	AARect getAABB() const
	{
		Vec2 corners[4];

		Mat2 inverse = this->rotationMat.inverse();

		corners[0] = center + Vec2(halfSize.x + radius, halfSize.y + radius) * inverse;
		corners[1] = center + Vec2(halfSize.x + radius, -halfSize.y - radius) * inverse;
		corners[2] = center + Vec2(-halfSize.x - radius, -halfSize.y - radius) * inverse;
		corners[3] = center + Vec2(-halfSize.x - radius, halfSize.y + radius) * inverse;


		Vec2 minPoint = corners[0];
		Vec2 maxPoint = corners[0];
		for (int i = 1; i < 4; i++)
		{
			minPoint.x = min(minPoint.x, corners[i].x);
			minPoint.y = min(minPoint.y, corners[i].y);
			maxPoint.x = max(maxPoint.x, corners[i].x);
			maxPoint.y = max(maxPoint.y, corners[i].y);
		}

		return AARect(minPoint, maxPoint);
	}
};