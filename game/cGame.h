#pragma once

#include "cMat.h"
#include "cResources.h"
#include "cShader.h"

class cRenderable;

class cGame
{
	Mat3 worldViewMatrix;
	Vec2 cameraPos;
	float cameraAngle;
	float cameraZoom;

	void renderInternal();
	void tickInternal();
	void initInternal();

	friend void RunGame();
	friend void InitGame();
	friend class Coral;

	friend class cRenderableWithShader;
	cShaderShr lastShader;

	std::vector<cRenderable*> renderables;
public:
	virtual void init() {}
	virtual void render() {}
	virtual void tick(float dt) {}

	void setCameraPos(const Vec2& newPos);
	void setCameraAngle(float angle);
	void setCameraZoom(float zoom);

	IntVec2 getScreenDimensions() const;

	void addRenderable(cRenderable* renderable);

};