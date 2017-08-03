#include "CollisionController.h"
#include "Bloodworks.h"
#include "Player.h"

CollisionController::CollisionController(Bloodworks *bloodworks)
{
	this->bloodworks = bloodworks;

	AARect boundaries = bloodworks->getMapLimits();
	boundaries.addThreshold(200.0f);

	bodyGrid.init(boundaries, 90.0f);
	return;
	const AARect& rect = bloodworks->getMapLimits();

	Circle playerCircle(Vec2::zero(), 40.0f);

	for (int i = 0; i < 40; i++)
	{
		Circle c(rect.getRandomPos(), randFloat(10.0f, 30.0f));
		if (c.doesIntersect(playerCircle))
		{
			i--;
			continue;
		}
		bodyGrid.insertBody(c, nullptr);
	}
	for (int i = 0; i < 40; i++)
	{
		Vec2 pos0 = rect.getRandomPos();
		Vec2 pos1 = pos0 + Vec2::fromAngle(randFloat(pi_2)) * randFloat(30.0f, 150.0f);
		Capsule c(pos0, pos1, randFloat(10.0f, 30.0f));
		if (c.doesIntersect(playerCircle))
		{
			i--;
			continue;
		}
		bodyGrid.insertBody(c, nullptr);
	}
	for (int i = 0; i < 40; i++)
	{
		Vec2 pos = rect.getRandomPos();
		Vec2 size(randFloat(20.0f, 150.0f), randFloat(20.0f, 150.0f));
		Rect c(pos, size, randFloat(pi_2));
		if (c.doesIntersect(playerCircle))
		{
			i--;
			continue;
		}
		bodyGrid.insertBody(c, nullptr);
	}
}

void CollisionController::reset()
{
	bodyGrid.reset();
}

void CollisionController::drawDebug(bool drawGrid)
{
	bodyGrid.drawDebug(drawGrid);
}

Vec2 CollisionController::getLongestSolver(const Circle& c)
{
	return bodyGrid.getLongestSolver(c);
}