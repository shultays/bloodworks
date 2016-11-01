#include "cGame.h"
#include "cTools.h"
#include "cGlobals.h"
#include "stdio.h"
#include "cResources.h"
#include "cRenderable.h"

extern SDL_Window *mainWindow;
extern SDL_GLContext mainContext;

void cGame::initInternal()
{
	cameraPos.setZero();
	cameraZoom = 1.0f;
	cameraAngle = 0.0f;


	init();

}

void cGame::tickInternal()
{
	tick(time.getDt());
}

void cGame::render()
{
	Vec2 windowSize = getScreenDimensions().toVec();
	float halfWidth = windowSize.w * 0.5f;
	float halfHeight = windowSize.h * 0.5f;
	worldViewMatrix = 
		Mat3::translationMatrix(-halfWidth - cameraPos.x, -halfHeight + cameraPos.y)
		.scaleBy(1.0f / windowSize.w, 1.0f / windowSize.h)
		.translateBy(0.5f)
		.scaleBy(2.0f);

	for (auto& renderable : renderables)
	{
		renderable->render();
	}

}

void cGame::setCameraPos(const Vec2& newPos)
{
	cameraPos = newPos;
}

void cGame::setCameraAngle(float angle)
{
	cameraAngle = angle;
}

void cGame::setCameraZoom(float zoom)
{
	cameraZoom = zoom;
}

IntVec2 cGame::getScreenDimensions() const
{
	IntVec2 ret;
	SDL_GetWindowSize(mainWindow, &ret.w, &ret.h);
	return ret;
}

void cGame::addRenderable(cRenderable* renderable)
{
	renderables.push_back(renderable);
}
