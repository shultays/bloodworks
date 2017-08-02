#include "CollisionController.h"
#include "Bloodworks.h"

CollisionController::CollisionController(Bloodworks *bloodworks)
{
	this->bloodworks = bloodworks;

	AARect boundaries = bloodworks->getMapLimits();
	boundaries.addThreshold(200.0f);

	bodyGrid.init(boundaries, 200.0f);
}
