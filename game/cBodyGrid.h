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
		void* userData;
		IntVec2 minGrid;
		IntVec2 maxGrid;
	};
	cVector<CircleData> circles;

	struct RectData
	{
		Rect rect;
		void* userData;
		IntVec2 minGrid;
		IntVec2 maxGrid;
	};
	cVector<RectData> rects;

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
	}

	int insertCircle(const Circle& circle, void* userData)
	{
		int index = circles.size();
		circles.resize(index + 1);
		CircleData& obj = circles[index];
		obj.circle = circle;
		AARect rect = circle.getAABB();
		obj.minGrid = getNodeIndex(rect.getMin());
		obj.maxGrid = getNodeIndex(rect.getMax());

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
		int index = rects.size();
		rects.resize(index + 1);
		RectData& obj = rects[index];
		obj.rect = rect;
		AARect aaRect = rect.getAABB();
		obj.minGrid = getNodeIndex(aaRect.getMin());
		obj.maxGrid = getNodeIndex(aaRect.getMax());

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
};