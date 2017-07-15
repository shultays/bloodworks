#pragma once

#include "cVec.h"

class Rect
{
	union
	{
		struct  
		{
			Vec2 min;
			Vec2 max;
		};
		Vec4 v;
	};
public:
	Rect(){}
	Rect(float minX, float minY, float maxX, float maxY)
	{
		min = Vec2(minX, minY);
		max = Vec2(maxX, maxY);
	}

	Rect(const Vec2& min, const Vec2& max)
	{
		this->min = min;
		this->max = max;
	}

	Rect(const Vec4& v)
	{
		this->v = v;
	}

	const Vec4& toVec() const
	{
		return v;
	}

	const static Rect& invalid()
	{
		static Rect r(0.0f, 0.0f, -1.0f, -1.0f);
		return r;
	}

	bool isInvalid() const
	{
		return min.x > max.x;
	}

	bool isValid() const
	{
		return min.x <= max.x;
	}

	bool isOutside(float x, float y) const
	{
		return x < min.x || y < min.x || x > max.x || y > max.y;
	}

	bool isOutside(const Vec2& pos) const
	{
		return isOutside(pos.x, pos.y);
	}

	bool isInside(const Vec2& pos) const
	{
		return !isOutside(pos);
	}
};