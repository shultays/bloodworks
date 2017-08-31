#pragma once

#include "cTools.h"

class AARect
{
public:
	union
	{
		struct  
		{
			Vec2 minPoint;
			Vec2 maxPoint;
		};
		Vec4 points;
	};

	AARect(){}
	AARect(float minX, float minY, float maxX, float maxY)
	{
		minPoint = Vec2(minX, minY);
		maxPoint = Vec2(maxX, maxY);
	}

	AARect(const Vec2& min, const Vec2& max)
	{
		this->minPoint = min;
		this->maxPoint = max;
	}

	AARect(const Vec4& v)
	{
		this->points = v;
	}

	const Vec4& toVec() const
	{
		return points;
	}

	const static AARect& invalid()
	{
		static AARect r(0.0f, 0.0f, -1.0f, -1.0f);
		return r;
	}

	bool isInvalid() const
	{
		return minPoint.x > maxPoint.x;
	}

	bool isValid() const
	{
		return minPoint.x <= maxPoint.x;
	}

	bool isValidSafeSafe() const
	{
		return minPoint.x <= maxPoint.x && minPoint.y <= maxPoint.y;
	}

	bool isOutside(float x, float y, float threshold) const
	{
		return x < minPoint.x - threshold || y < minPoint.y - threshold || x > maxPoint.x  + threshold || y > maxPoint.y + threshold;
	}

	bool isOutside(float x, float y) const
	{
		return x < minPoint.x || y < minPoint.y || x > maxPoint.x || y > maxPoint.y;
	}

	bool isOutside(const Vec2& pos) const
	{
		return isOutside(pos.x, pos.y);
	}

	bool isOutside(const Vec2& pos, float threshold) const
	{
		return isOutside(pos.x, pos.y, threshold);
	}

	bool isInside(const Vec2& pos) const
	{
		return !isOutside(pos);
	}

	bool isInside(const Vec2& pos, float threshold) const
	{
		return !isOutside(pos, threshold);
	}

	bool operator==(const AARect& other) const
	{
		return points == other.points;
	}

	void operator=(const AARect& other)
	{
		points = other.points;
	}

	AARect transform(const Mat3& mat) const;

	const Vec2& getMin() const
	{
		return minPoint;
	}

	const Vec2& getMax() const
	{
		return maxPoint;
	}

	void setMin(const Vec2& min)
	{
		this->minPoint = min;
	}

	void setMax(const Vec2& max)
	{
		this->maxPoint = max;
	}

	Vec2 getMid() const
	{
		return (minPoint + maxPoint) * 0.5f;
	}

	void set(const Vec2& min, const Vec2& max)
	{
		this->minPoint = min;
		this->maxPoint = max;
	}

	void addThreshold(float threshold)
	{
		minPoint -= threshold;
		maxPoint += threshold;
	}

	void addThreshold(float horizontal, float vertical)
	{
		minPoint.x -= horizontal;
		maxPoint.x += horizontal;
		minPoint.y -= vertical;
		maxPoint.y += vertical;
	}


	Vec2 getSize() const
	{
		return maxPoint - minPoint;
	}

	void clampPos(Vec2& position) const;
	Vec2 getRandomPos() const;
	bool doesIntersect(const AARect& rect) const;
};