#pragma once

#include "cTools.h"
#include "Array2d.h"
#include "cCircle.h"
#include "cRect.h"
#include "cVector.h"
#include <vector>

#define RECT_START 0x1000000

class cBodyGrid
{

	Vec2 gridStart;
	Vec2 gridSize;
	Vec2 nodeSize;
	IntVec2 nodeCount;

	Array2d<cVector<int>> data;

	struct CircleData 
	{
		Circle circle;
		AARect aabb;
		void* userData;
		IntVec2 minGrid;
		IntVec2 maxGrid;
	};
	cVector<CircleData> circles;
	cVector<int> removedCircleIndices;

	struct RectData
	{
		Rect rect;
		AARect aabb;
		void* userData;
		IntVec2 minGrid;
		IntVec2 maxGrid;
	};
	cVector<RectData> rects;
	cVector<int> removedRectIndices;

	IntVec2 getNodeIndex(const Vec2& pos) const
	{
		Vec2 diff = pos - gridStart;
		return IntVec2((int)floor(diff.x / nodeSize.x), (int)floor(diff.y / nodeSize.y));
	}

	int arrayIndexForNodeIndex(const IntVec2& nodeIndex)
	{
		return nodeIndex.y * nodeCount.x + nodeIndex.x;
	}
public:
	cBodyGrid()
	{
	}
	void init(const AARect& rect, const Vec2& nodeSize)
	{
		init(rect.getMin(), rect.getSize(), nodeSize);
	}


	void init(const Vec2& gridStart, const Vec2& gridSize, const Vec2& nodeSize)
	{
		this->gridStart = gridStart;
		this->nodeSize = nodeSize;

		nodeCount.x = (int)ceil(gridSize.x / nodeSize.x);
		nodeCount.y = (int)ceil(gridSize.y / nodeSize.y);

		this->gridSize.x = nodeCount.x * nodeSize.x;
		this->gridSize.y = nodeCount.y * nodeSize.y;

		data.init(nodeCount);
	}

	void removeCircle(int index)
	{
		CircleData& obj = circles[index];

		for (int x = obj.minGrid.x; x <= obj.maxGrid.x; x++)
		{
			for (int y = obj.minGrid.y; y <= obj.maxGrid.y; y++)
			{
				data[x][y].swapToTailRemoveElement(index);
			}
		}

		removedCircleIndices.push_back(index);
	}

	void removeRect(int index)
	{
		RectData& obj = rects[index];

		for (int x = obj.minGrid.x; x <= obj.maxGrid.x; x++)
		{
			for (int y = obj.minGrid.y; y <= obj.maxGrid.y; y++)
			{
				data[x][y].swapToTailRemoveElement(index + RECT_START);
			}
		}

		removedRectIndices.push_back(index);
	}

	int insertCircle(const Circle& circle, void* userData)
	{
		int index;
		if (removedCircleIndices.size() == 0)
		{
			index = circles.size();
			circles.resize(index + 1);
		}
		else
		{
			index = removedCircleIndices[removedCircleIndices.size() - 1];
			removedCircleIndices.resize(removedCircleIndices.size() - 1);
		}
		CircleData& obj = circles[index];
		obj.circle = circle;
		obj.aabb = circle.getAABB();
		obj.minGrid = getNodeIndex(obj.aabb.getMin());
		obj.maxGrid = getNodeIndex(obj.aabb.getMax());

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

	int insertRect(const Rect& rect, void* userData)
	{
		int index;
		if (removedRectIndices.size() == 0)
		{
			index = rects.size();
			rects.resize(index + 1);
		}
		else
		{
			index = removedRectIndices[removedRectIndices.size() - 1];
			removedRectIndices.resize(removedRectIndices.size() - 1);
		}
		RectData& obj = rects[index];
		obj.rect = rect;
		obj.aabb = rect.getAABB();
		obj.minGrid = getNodeIndex(obj.aabb.getMin());
		obj.maxGrid = getNodeIndex(obj.aabb.getMax());

		for (int x = obj.minGrid.x; x <= obj.maxGrid.x; x++)
		{
			for (int y = obj.minGrid.y; y <= obj.maxGrid.y; y++)
			{
				data[x][y].push_back(index + RECT_START);
			}
		}

		obj.userData = userData;
		return index;
	}

	void relocateCircle(int index)
	{
		CircleData& obj = circles[index];

		obj.aabb = obj.circle.getAABB();
		IntVec2 minGrid = getNodeIndex(obj.aabb.getMin());
		IntVec2 maxGrid = getNodeIndex(obj.aabb.getMax());

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
	void relocateRect(int index)
	{
		RectData& obj = rects[index];

		obj.aabb = obj.rect.getAABB();
		IntVec2 minGrid = getNodeIndex(obj.aabb.getMin());
		IntVec2 maxGrid = getNodeIndex(obj.aabb.getMax());

		if (obj.minGrid != minGrid || obj.maxGrid != maxGrid)
		{
			for (int x = obj.minGrid.x; x <= obj.maxGrid.x; x++)
			{
				for (int y = obj.minGrid.y; y <= obj.maxGrid.y; y++)
				{
					if (x < minGrid.x || x > maxGrid.x || y < minGrid.y || y > maxGrid.y)
					{
						auto& v = data[x][y];
						v.swapToTailRemoveElement(index + RECT_START);
					}
				}
			}

			for (int x = minGrid.x; x <= maxGrid.x; x++)
			{
				for (int y = minGrid.y; y <= maxGrid.y; y++)
				{
					if (x < obj.minGrid.x || x > obj.maxGrid.x || y < obj.minGrid.y || y > obj.maxGrid.y)
					{
						data[x][y].push_back(index + RECT_START);
					}
				}
			}

			obj.minGrid = minGrid;
			obj.maxGrid = maxGrid;
		}
	}
};