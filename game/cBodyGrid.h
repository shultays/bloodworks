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
	Vec2 gridSize;
	Vec2 nodeSize;
	IntVec2 nodeCount;

	Array2d<cVector<int>> data;


	IntVec2 getNodeIndex(const Vec2& pos) const
	{
		Vec2 diff = pos - gridStart;
		return IntVec2((int)floor(diff.x / nodeSize.x), (int)floor(diff.y / nodeSize.y));
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

	Vec2 getLongestSolver(const Circle& c)
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
					if (body.lastSearchIndex == searchIndex)
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


};