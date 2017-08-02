#pragma once

#include "cTools.h"
#include "cCircle.h"
#include "cGlobals.h"

class Rect
{
	Vec2 center;
	Vec2 halfSize;

	float angle;
	Mat2 rotationMat;

public:
	Rect(){}
	Rect(const Vec2& center, const Vec2& halfSize, float angle)
	{
		this->center = center;
		this->halfSize = halfSize;
		this->angle = angle;
		rotationMat = Mat2::rotation(-angle);
	}

	bool doesIntersect(const Vec2& point) const
	{
		Vec2 diff = point - center;
		Vec2 transformed = diff * rotationMat;

		return transformed.x > -halfSize.x
			&& transformed.y > -halfSize.y
			&& transformed.x < +halfSize.x
			&& transformed.y < +halfSize.y;
	}

	bool doesIntersect(const Circle& circle) const
	{
		Vec2 diff = circle.getOrigin() - center;
		Vec2 transformed = diff * rotationMat;
		float radius = circle.getRadius();
		return transformed.x > -halfSize.x - radius
			&& transformed.y > -halfSize.y - radius
			&& transformed.x < +halfSize.x + radius
			&& transformed.y < +halfSize.y + radius;
	}


	Vec2 getSolver(const Circle& circle) const
	{
		Mat2 inverted = rotationMat.inverse();
		Vec2 diff = circle.getOrigin() - center;
		Vec2 transformed = diff * rotationMat;
		float radius = circle.getRadius();

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

			float d = radius * radius - diffX * diffX - diffY * diffY;
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
					return Vec2(diffX + radius, 0.0f) * inverted;
				}
				else
				{
					return -Vec2(diffX + radius, 0.0f) * inverted;
				}
			}
			else
			{
				if (transformed.y > 0.0f)
				{
					return Vec2(0.0f, diffY + radius) * inverted;
				}
				else
				{
					return -Vec2(0.0f, diffY + radius) * inverted;
				}
			}
		}
		else
		{
			Vec2 toCircle = transformed - closestPoint;
			float l = toCircle.normalize();
			if (l < radius)
			{
				return toCircle * (radius - l) * inverted;
			}
		}


		return Vec2::zero();
	}

	void drawDebug(int color = 0xFFFFFFFF, float time = 0.0f) const
	{
		Vec2 corners[4];

		Mat2 rotationMat = Mat2::rotation(angle);

		corners[0] = center + Vec2(halfSize.x, halfSize.y) * rotationMat;
		corners[1] = center + Vec2(halfSize.x, -halfSize.y) * rotationMat;
		corners[2] = center + Vec2(-halfSize.x, -halfSize.y) * rotationMat;
		corners[3] = center + Vec2(-halfSize.x, halfSize.y) * rotationMat;

		debugRenderer.addLine(corners[0], corners[1], time, color);
		debugRenderer.addLine(corners[1], corners[2], time, color);
		debugRenderer.addLine(corners[2], corners[3], time, color);
		debugRenderer.addLine(corners[3], corners[0], time, color);
		
	}

	AARect getAABB() const
	{
		Mat2 inverted = rotationMat.inverse();
		Vec2 rotateHalfSize = halfSize * inverted;
		AARect r(center - rotateHalfSize, center + rotateHalfSize);
		return r;
	}


};