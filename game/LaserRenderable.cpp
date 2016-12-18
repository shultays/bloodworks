#include "LaserRenderable.h"
#include "cTexture.h"


LaserRenderable::LaserRenderable(Bloodworks *bloodworks) : cRenderableGroup((cGame*)bloodworks)
{
	laserLength = 100.0f;
	laserScale = 20.0f;

	laserBeginRenderable = new cTexturedQuadRenderable(game, "resources/guns/laser/begin.png", "resources/default");
	this->addRenderable(laserBeginRenderable);
	laserMidrenderable = new cTexturedQuadRenderable(game, "resources/guns/laser/mid.png", "resources/default");
	this->addRenderable(laserMidrenderable);
	laserEndRenderable = new cTexturedQuadRenderable(game, "resources/guns/laser/end.png", "resources/default");
	this->addRenderable(laserEndRenderable);

	laserBeginShift = Vec2(-10.0f, 0.0f);
	laserEndShift = Vec2(10.0f, 0.0f);
	updateRenderables();
	
	debugRenderer.addCircle(Vec2::zero(), 5.0f, debugRenderer.infinite);
	debugRenderer.addCircle(Vec2(laserLength, 0.0f), 5.0f, debugRenderer.infinite);
}



void LaserRenderable::updateRenderables()
{
	laserBeginRenderable->setWorldMatrix(Mat3::scaleMatrix(laserScale).translateBy(laserBeginShift - Vec2(laserBeginRenderable->getTexture()->getDimensions().x * 0.5f / laserScale, 0.0f)));
	laserMidrenderable->setWorldMatrix(Mat3::scaleMatrix(Vec2(laserLength * 0.5f - laserEndShift.x, laserScale)).translateBy(Vec2(laserLength * 0.5f, 20.0f)));
	laserEndRenderable->setWorldMatrix(Mat3::scaleMatrix(laserScale).translateBy(laserEndShift + Vec2(laserLength + laserEndRenderable->getTexture()->getDimensions().x * 0.5f / laserScale, 0.0f)));
}
