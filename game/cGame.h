#pragma once

#include "cMat.h"
#include "cResources.h"
#include "cPriorityVector.h"

class cRenderable;

class cGame
{
protected:
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

	cPriorityVector<cRenderable*> renderables;
public:
	cShaderShr lastShader;
	virtual void init() {}
	virtual void render() {}
	virtual void tick(float dt) {}
	virtual ~cGame()
	{
		lastShader = nullptr;
		if (renderables.size() > 0)
		{
			assert("there are some non-removed renderables");
		}
	}

	void setCameraPos(const Vec2& newPos);
	void setCameraAngle(float angle);
	void setCameraZoom(float zoom);

	IntVec2 getScreenDimensions() const;

	void addRenderable(cRenderable* renderable, int level);

	void removeRenderable(cRenderable* renderable);

	const Mat3& getViewMatrix()
	{
		return worldViewMatrix;
	}
};