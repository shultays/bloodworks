#include "CollisionController.h"
#include "Bloodworks.h"
#include "Player.h"

CollisionController::CollisionController(Bloodworks *bloodworks)
{
	this->bloodworks = bloodworks;

	updateGrid();
}

void CollisionController::reset()
{
	bodyGrid.reset();
}

void CollisionController::drawDebug(bool drawGrid)
{
	bodyGrid.drawDebug(drawGrid);
}

Vec2 CollisionController::getLongestSolver(const Circle& c, unsigned ignoreFlags)
{
	return bodyGrid.getLongestSolver(c, ignoreFlags);
}

Vec2 CollisionController::getFreePosition(float radius, unsigned ignoreFlags)
{
	const AARect rect = bloodworks->getMapLimits();
	for (int i = 0; i < 16; i++)
	{
		Vec2 pos = rect.getRandomPos();
		if (bodyGrid.hasCollision(Circle(pos, radius), ignoreFlags))
		{
			continue;
		}
		return pos;
	}
	return rect.getRandomPos();
}

float CollisionController::getRayDistance(const Vec2& begin, const Vec2& ray, float radius, unsigned ignoreFlags)
{
	return bodyGrid.getRayDistance(begin, ray, radius, ignoreFlags);
}

void CollisionController::updateGrid()
{
	AARect boundaries = bloodworks->getMapLimits();
	boundaries.addThreshold(200.0f);

	bodyGrid.init(boundaries, 90.0f);
}
