#pragma once

#include "cMat.h"
#include "cResources.h"
#include "cPriorityVector.h"

enum class RenderableAlignment
{
	world,
	topLeft,
	top,
	topRight,
	left,
	center,
	right,
	bottomLeft,
	bottom,
	bottomRight,
	count
};

class cRenderable;

class cGame
{
protected:
	Mat3 viewMatrices[(int)RenderableAlignment::count];
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

	cRenderable *first;
public:
	cShaderShr lastShader;
	RenderableAlignment lastAllignment;

	virtual void init() {}
	virtual void render() {}
	virtual void tick(float dt) {}
	virtual ~cGame();

	void setCameraPos(const Vec2& newPos);
	void setCameraAngle(float angle);
	void setCameraZoom(float zoom);

	IntVec2 getScreenDimensions() const;

	void addRenderable(cRenderable* renderable, int level);

	void removeRenderable(cRenderable* renderable);

	const Mat3& getViewMatrix(RenderableAlignment alignment)
	{
		return viewMatrices[(int)alignment];
	}
};