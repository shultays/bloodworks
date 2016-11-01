#pragma once

#include "cMat.h"
#include "cResources.h"

class cRenderable;

class cGame
{
	Mat3 worldViewMatrix;
	Vec2 cameraPos;
	float cameraAngle;
	float cameraZoom;

	void render();
	void tickInternal();
	void initInternal();

	friend void RunGame();
	friend void InitGame();
	friend class Coral;

	friend class cRenderableWithShader;
	cShaderShr lastShader;

	std::vector<cRenderable*> renderables;
public:
	virtual void init() = 0;
	virtual void tick(float dt) = 0;

	void setCameraPos(const Vec2& newPos);
	void setCameraAngle(float angle);
	void setCameraZoom(float zoom);

	IntVec2 getScreenDimensions() const;

	void addRenderable(cRenderable* renderable);

};