#pragma once

#include "cTools.h"
#include "Array2d.h"
#include "cCapsule.h"
#include "cCircle.h"
#include "cRect.h"
#include "cVector.h"
#include <vector>

#define RECT_START 0x1000000
#define CAPSULE_START 0x2000000

class cBodyGrid
{

	struct BodyUnion
	{
		BodyUnion(){}
		enum
		{
			type_invalid,
			type_circle,
			type_rect,
			type_capsule
		};
		int bodyType;
		int lastSearchIndex;
		void* userData;
		IntVec2 minGrid;
		IntVec2 maxGrid;
		unsigned flags;
		union
		{
			Circle circle;
			Rect rect;
			Capsule capsule;
		};

		void set(const Circle& circle)
		{
			this->circle = circle;
			bodyType = type_circle;
			lastSearchIndex = 0;
		}

		void set(const Rect& rect)
		{
			this->rect = rect;
			bodyType = type_rect;
			lastSearchIndex = 0;
		}

		void set(const Capsule& capsule)
		{
			this->capsule = capsule;
			bodyType = type_capsule;
			lastSearchIndex = 0;
		}

		AARect getAABB() const
		{
			if (bodyType == type_circle)
			{
				return circle.getAABB();
			}
			else if (bodyType == type_rect)
			{
				return rect.getAABB();
			}
			else if (bodyType == type_capsule)
			{
				return capsule.getAABB();
			}
			return AARect::invalid();
		}
	};

	cVector<BodyUnion> bodies;
	cVector<int> removedBodyIndices;

	Vec2 initialGridSize;
	Vec2 gridStart;
	Vec2 gridEnd;
	Vec2 gridSize;
	Vec2 nodeSize;
	IntVec2 nodeCount;

	Array2d<cVector<int>> data;

	void clampIndex(IntVec2& index) const
	{
		index.x = max(0, index.x);
		index.y = max(0, index.y);

		index.x = min(nodeCount.x - 1, index.x);
		index.y = min(nodeCount.y - 1, index.y);
	}

	IntVec2 getNodeIndex(const Vec2& pos) const
	{
		Vec2 diff = pos - gridStart;
		IntVec2 v((int)floor(diff.x / nodeSize.x), (int)floor(diff.y / nodeSize.y));
		clampIndex(v);
		return v;
	}

	IntVec2 getNodeIndexUnSafe(const Vec2& pos) const
	{
		Vec2 diff = pos - gridStart;
		IntVec2 v((int)floor(diff.x / nodeSize.x), (int)floor(diff.y / nodeSize.y));
		return v;
	}

	int arrayIndexForNodeIndex(const IntVec2& nodeIndex)
	{
		return nodeIndex.y * nodeCount.x + nodeIndex.x;
	}

	int nextSearchIndex;
	int getSearchIndex()
	{
		nextSearchIndex++;
		if (nextSearchIndex == 0)
		{
			for (auto& body : bodies)
			{
				body.lastSearchIndex = 0;
			}
			nextSearchIndex = 1;
		}
		return nextSearchIndex;
	}
public:
	cBodyGrid()
	{
		nextSearchIndex = 0;
	}
	void init(const AARect& rect, const Vec2& nodeSize)
	{
		init(rect.getMin(), rect.getSize(), nodeSize);
	}


	void init(const Vec2& gridStart, const Vec2& gridSize, const Vec2& nodeSize)
	{
		this->gridStart = gridStart;
		this->nodeSize = nodeSize;
		this->initialGridSize = gridSize;

		nodeCount.x = (int)ceil(gridSize.x / nodeSize.x);
		nodeCount.y = (int)ceil(gridSize.y / nodeSize.y);

		this->gridSize.x = nodeCount.x * nodeSize.x;
		this->gridSize.y = nodeCount.y * nodeSize.y;

		data.init(nodeCount);
	}

	void removeBody(int index)
	{
		BodyUnion& obj = bodies[index];

		for (int x = obj.minGrid.x; x <= obj.maxGrid.x; x++)
		{
			for (int y = obj.minGrid.y; y <= obj.maxGrid.y; y++)
			{
				data[x][y].swapToTailRemoveElement(index);
			}
		}
		obj.bodyType = BodyUnion::type_invalid;
		removedBodyIndices.push_back(index);
	}

	template<class T>
	int insertBody(const T& body, void* userData)
	{
		int index;
		if (removedBodyIndices.size() == 0)
		{
			index = bodies.size();
			bodies.resize(index + 1);
		}
		else
		{
			index = removedBodyIndices[removedBodyIndices.size() - 1];
			removedBodyIndices.resize(removedBodyIndices.size() - 1);
		}
		BodyUnion& obj = bodies[index];
		obj.flags = 0;
		obj.set(body);
		AARect aabb = obj.getAABB();
		obj.minGrid = getNodeIndex(aabb.getMin());
		obj.maxGrid = getNodeIndex(aabb.getMax());

		for (int x = obj.minGrid.x; x <= obj.maxGrid.x; x++)
		{
			for (int y = obj.minGrid.y; y <= obj.maxGrid.y; y++)
			{
				data[x][y].push_back(index);
			}
		}

		obj.userData = userData;
		return index;
	}

	template<class T>
	bool hasCollision(const T& testBody, unsigned ignoreFlags)
	{
		AARect aabb = testBody.getAABB();
		IntVec2 minGrid = getNodeIndex(aabb.getMin());
		IntVec2 maxGrid = getNodeIndex(aabb.getMax());
		int searchIndex = getSearchIndex();
		for (int x = minGrid.x; x <= maxGrid.x; x++)
		{
			for (int y = minGrid.y; y <= maxGrid.y; y++)
			{
				for (auto i : data[x][y])
				{
					BodyUnion& body = bodies[i];
					if (body.lastSearchIndex == searchIndex || (body.flags & ignoreFlags) > 0)
					{
						continue;
					}
					body.lastSearchIndex = searchIndex;
					if (body.bodyType == BodyUnion::type_circle)
					{
						if (body.circle.doesIntersect(testBody))
						{
							return true;
						}
					}
					else if (body.bodyType == BodyUnion::type_capsule)
					{
						if (body.capsule.doesIntersect(testBody))
						{
							return true;
						}
					}
					else if (body.bodyType == BodyUnion::type_rect)
					{
						if (body.rect.doesIntersect(testBody))
						{
							return true;
						}
					}
				}
			}
		}
		return false;
	}

	template<class T>
	void relocateBody(int index, const T& body)
	{
		BodyUnion& obj = bodies[index];
		obj.set(body);
		AARect aabb = obj.getAABB();
		IntVec2 minGrid = getNodeIndex(aabb.getMin());
		IntVec2 maxGrid = getNodeIndex(aabb.getMax());

		if (obj.minGrid != minGrid || obj.maxGrid != maxGrid)
		{
			for (int x = obj.minGrid.x; x <= obj.maxGrid.x; x++)
			{
				for (int y = obj.minGrid.y; y <= obj.maxGrid.y; y++)
				{
					if (x < minGrid.x || x > maxGrid.x || y < minGrid.y || y > maxGrid.y)
					{
						auto& v = data[x][y];
						v.swapToTailRemoveElement(index);
					}
				}
			}

			for (int x = minGrid.x; x <= maxGrid.x; x++)
			{
				for (int y = minGrid.y; y <= maxGrid.y; y++)
				{
					if (x < obj.minGrid.x || x > obj.maxGrid.x || y < obj.minGrid.y || y > obj.maxGrid.y)
					{
						data[x][y].push_back(index);
					}
				}
			}

			obj.minGrid = minGrid;
			obj.maxGrid = maxGrid;
		}
	}

	void drawDebug(bool drawGrid = true)
	{
		if (drawGrid)
		{
			for (int x = 0; x < nodeCount.y; x++)
			{
				debugRenderer.addLine(
					Vec2(gridStart.x, gridStart.y + x * nodeSize.y),
					Vec2(gridStart.x + gridSize.w, gridStart.y + x * nodeSize.y),
					0.0f, 0x550000FF);
			}
			for (int y = 0; y < nodeCount.x; y++)
			{
				debugRenderer.addLine(
					Vec2(gridStart.x + y * nodeSize.x, gridStart.y),
					Vec2(gridStart.x + y * nodeSize.x, gridStart.y + gridSize.h),
					0.0f, 0x550000FF);
			}

			for (int x = 0; x < nodeCount.x - 1; x++)
			{
				for (int y = 0; y < nodeCount.y - 1; y++)
				{
					auto& v = data[x][y];

					if (v.size())
					{
						debugRenderer.addLine(gridStart + Vec2((float)x, (float)y) * nodeSize, gridStart + Vec2((float)x + 1, (float)y) * nodeSize, 0.0f, 0xFFFF0000);
						debugRenderer.addLine(gridStart + Vec2((float)x, (float)y) * nodeSize, gridStart + Vec2((float)x, (float)y + 1) * nodeSize, 0.0f, 0xFFFF0000);
						debugRenderer.addLine(gridStart + Vec2((float)x + 1, (float)y + 1) * nodeSize, gridStart + Vec2((float)x + 1, (float)y) * nodeSize, 0.0f, 0xFFFF0000);
						debugRenderer.addLine(gridStart + Vec2((float)x + 1, (float)y + 1) * nodeSize, gridStart + Vec2((float)x, (float)y + 1) * nodeSize, 0.0f, 0xFFFF0000);
					}
				}
			}
		}

		for (auto& body : bodies)
		{
			if (body.bodyType == BodyUnion::type_circle)
			{
				body.circle.drawDebug();
			}
			else if (body.bodyType == BodyUnion::type_rect)
			{
				body.rect.drawDebug();
			}
			else if (body.bodyType == BodyUnion::type_capsule)
			{
				body.capsule.drawDebug();
			}
		}
	}

	Vec2 getLongestSolver(const Circle& c, unsigned ignoreFlags)
	{
		AARect rect = c.getAABB();

		IntVec2 minGrid = getNodeIndex(rect.getMin());
		IntVec2 maxGrid = getNodeIndex(rect.getMax());
		Vec2 r;
		r.setZero();
		float longestDistance = 0.0f;
		int searchIndex = getSearchIndex();
		for (int x = minGrid.x; x <= maxGrid.x; x++)
		{
			for (int y = minGrid.y; y <= maxGrid.y; y++)
			{
				for (auto i : data[x][y])
				{
					Vec2 cur;
					BodyUnion& body = bodies[i];
					if (body.lastSearchIndex == searchIndex || (body.flags & ignoreFlags) > 0)
					{
						continue;
					}
					body.lastSearchIndex = searchIndex;
					if (body.bodyType == BodyUnion::type_circle)
					{
						cur = body.circle.getSolver(c);
					}
					else if (body.bodyType == BodyUnion::type_rect)
					{
						cur = body.rect.getSolver(c);
					}
					else if (body.bodyType == BodyUnion::type_capsule)
					{
						cur = body.capsule.getSolver(c);
					}
					float d = cur.lengthSquared();
					if (longestDistance < d)
					{
						longestDistance = d;
						r = cur;
					}
				}
			}
		}

		return r;
	}

	void reset()
	{
		init(gridStart, initialGridSize, nodeSize);
		bodies.clear();
		removedBodyIndices.clear();
	}


	float getRayDistance(const Vec2& begin, const Vec2& ray, float radius, int ignoreFlags)
	{
		float distance = ray.length();

		std::function<bool(int)> func = [&](int index)
		{
			float r = -1.0f;
			BodyUnion& body = bodies[index];
			if (body.bodyType == BodyUnion::type_circle)
			{
				r = body.circle.doesHit(begin, ray, radius);
			}
			else if (body.bodyType == BodyUnion::type_rect)
			{
				r = body.rect.doesHit(begin, ray, radius);
			}
			else if (body.bodyType == BodyUnion::type_capsule)
			{
				r = body.capsule.doesHit(begin, ray, radius);
			}

			if (r >= 0.0f && r < distance)
			{
				distance = r;
			}
			return false;
		};

		runForRay(begin, ray, radius, ignoreFlags, func);

		return distance;
	}

	bool runForNode(const IntVec2& index, int searchId, int ignoreFlags, std::function<bool(int)>& func)
	{
		if (data.isValid(index) == false)
		{
			return false;
		}
		auto& node = data[index];
		for (int i : node)
		{
			if (bodies[i].lastSearchIndex == searchId || (bodies[i].flags & ignoreFlags) > 0)
			{
				continue;
			}
			bodies[i].lastSearchIndex = searchId;

			if (func(i))
			{
				return true;
			}
		}

		return false;
	}

	bool runForRay(const Vec2& begin, const Vec2& ray, float radius, int ignoreFlags, std::function<bool(int)>& func)
	{
		bool renderDebug = input.isKeyDown(key_g);

		if (radius * radius > nodeSize.lengthSquared())
		{
			for (int i=0; i<bodies.size(); i++)
			{
				if (bodies[i].bodyType != BodyUnion::type_invalid && (bodies[i].flags & ignoreFlags) == 0)
				{
					if (func(i))
					{
						return true;
					}
				}
			}
		}
		else
		{
			int searchId = getSearchIndex();

			IntVec2 start = getNodeIndexUnSafe(begin);
			IntVec2 end = getNodeIndexUnSafe(begin + ray);

			IntVec2 dirInt(ray.x > 0.0f ? 1 : -1, ray.y > 0.0f ? 1 : -1);
			IntVec2 posShift(ray.x > 0.0f ? 1 : 0, ray.y > 0.0f ? 1 : 0);

			Vec2 currentPos = begin;
			IntVec2 current = start;

			int checkVal = abs(ray.x) < abs(ray.y) ? 1 : 0;

			bool alwaysHorizontal = abs(ray.y) < 0.0001f;
			bool alwaysVertical = abs(ray.x) < 0.0001f;

			bool checked[3][3];
			memset(checked, 0, sizeof(checked));

			int maxTry = 200;
			bool hasRadius = radius > 0.01f;
			bool finished = false;
			while (finished == false)
			{
				if (current == end)
				{
					finished = true;
				}
				if (checked[1][1] == false || hasRadius == false)
				{
					checked[1][1] = true;
					if (runForNode(current, searchId, ignoreFlags, func))
					{
						return true;
					}
				}

				Vec2 nextPos = gridStart + nodeSize * (current + posShift).toVec();
				if (renderDebug)
				{
					debugRenderer.addCircle(currentPos, 3.0f, 0.0f, 0xFF00FFFF);
				}
				if (hasRadius)
				{
					Vec2 curPos = gridStart + nodeSize * (current).toVec();
					Vec2 endPos = gridStart + nodeSize * (current + 1).toVec();

					if (currentPos.x - radius < curPos.x)
					{
						if (checked[0][1] == false)
						{
							checked[0][1] = true;
							if (runForNode(current + IntVec2(-1, 0), searchId, ignoreFlags, func))
							{
								return true;
							}
						}
						if (checked[0][0] == false && currentPos.y - radius < curPos.y)
						{
							checked[0][0] = true;
							if (runForNode(current + IntVec2(-1, -1), searchId, ignoreFlags, func))
							{
								return true;
							}
						}
						if (checked[0][2] == false && currentPos.y + radius > endPos.y)
						{
							checked[0][2] = true;
							if (runForNode(current + IntVec2(-1, +1), searchId, ignoreFlags, func))
							{
								return true;
							}
						}
					}

					if (checked[1][0] == false && currentPos.y - radius < curPos.y)
					{
						checked[1][0] = true;
						if (runForNode(current + IntVec2(0, -1), searchId, ignoreFlags, func))
						{
							return true;
						}
					}
					if (checked[1][2] == false && currentPos.y + radius > endPos.y)
					{
						checked[1][2] = true;
						if (runForNode(current + IntVec2(0, +1), searchId, ignoreFlags, func))
						{
							return true;
						}
					}

					if (currentPos.x + radius > endPos.x)
					{
						if (checked[2][1] == false)
						{
							checked[2][1] = true;
							if (runForNode(current + IntVec2(+1, 0), searchId, ignoreFlags, func))
							{
								return true;
							}
						}
						if (checked[2][0] == false && currentPos.y - radius < curPos.y)
						{
							checked[2][0] = true;
							if (runForNode(current + IntVec2(+1, -1), searchId, ignoreFlags, func))
							{
								return true;
							}
						}
						if (checked[2][2] == false && currentPos.y + radius > endPos.y)
						{
							checked[2][2] = true;
							if (runForNode(current + IntVec2(+1, +1), searchId, ignoreFlags, func))
							{
								return true;
							}
						}
					}
				}


				Vec2 d = (nextPos - begin) / ray;
				if (d[0] > 1.0f && d[1] > 1.0f)
				{
					break;
				}

				bool v = false;

				if (alwaysHorizontal)
				{
					current.x += dirInt.x;
					currentPos.x = nextPos.x;
					currentPos.y = begin.y + ((currentPos.x - begin.x) / ray.x) * ray.y;
				}
				else if (alwaysVertical)
				{
					current.y += dirInt.y;
					currentPos.y = nextPos.y;
					currentPos.x = begin.x + ((currentPos.y - begin.y) / ray.y) * ray.x;
					v = true;
				}
				else
				{
					Vec2 diff = (nextPos - currentPos);
					float dx = diff.x / ray.x;
					float dy = diff.y / ray.y;
					if (dx < dy)
					{
						current.x += dirInt.x;
						currentPos.x = nextPos.x;
						currentPos.y = begin.y + ((currentPos.x - begin.x) / ray.x) * ray.y;
					}
					else
					{
						current.y += dirInt.y;
						currentPos.y = nextPos.y;
						currentPos.x = begin.x + ((currentPos.y - begin.y) / ray.y) * ray.x;
						v = true;
					}
				}

				if (hasRadius)
				{
					if (v)
					{
						if (dirInt.y < 0)
						{
							for (int i = 0; i < 3; i++)
							{
								checked[i][2] = checked[i][1];
								checked[i][1] = checked[i][0];
								checked[i][0] = false;
							}
						}
						else
						{
							for (int i = 0; i < 3; i++)
							{
								checked[i][0] = checked[i][1];
								checked[i][1] = checked[i][2];
								checked[i][2] = false;
							}
						}
					}
					else
					{
						if (dirInt.x < 0)
						{
							for (int i = 0; i < 3; i++)
							{
								checked[2][i] = checked[1][i];
								checked[1][i] = checked[0][i];
								checked[0][i] = false;
							}
						}
						else
						{
							for (int i = 0; i < 3; i++)
							{
								checked[0][i] = checked[1][i];
								checked[1][i] = checked[2][i];
								checked[2][i] = false;
							}
						}
					}
				}
			}

			if (maxTry <= 0)
			{
				printf("Reached maxtry on ray cast, error?\n");
			}
			if (renderDebug)
			{
				Vec2 perp = ray.normalized().sideVec();
				//debugRenderer.addLine(begin, begin + ray, 0.0f, 0xFFFF0000);
				debugRenderer.addCircle(begin, radius, 0.0f, 0xFFFF0000);
				debugRenderer.addCircle(begin + ray, radius, 0.0f, 0xFFFF0000);
				debugRenderer.addLine(begin + perp * radius, begin + perp * radius + ray, 0.0f, 0xFFFF0000);
				debugRenderer.addLine(begin - perp * radius, begin - perp * radius + ray, 0.0f, 0xFFFF0000);
			}
		}
		return false;
	}

	void setFlag(int index, unsigned flag, bool isSet)
	{
		if (isSet)
		{
			bodies[index].flags |= flag;
		}
		else
		{
			bodies[index].flags &= ~flag;
		}
	}

	void setFlags(int index, unsigned flags)
	{
		bodies[index].flags = flags;
	}

	bool hasFlag(int index, unsigned flag)
	{
		return (bodies[index].flags & flag) > 0;
	}

	unsigned getFlags(int index)
	{
		return bodies[index].flags;
	}
};