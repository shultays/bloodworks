#pragma once

#include "cMat.h"
#include "cResources.h"
#include "cPriorityVector.h"
#include <GL/glew.h>

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
class cPostProcess;

class cGame
{
protected:
	Mat3 viewMatrices[(int)RenderableAlignment::count];
	Vec2 cameraPos;
	float cameraAngle;
	float cameraZoom;

	float slowdown;

	void renderInternal();
	void tickInternal();
	void initInternal();

	friend void RunGame();
	friend void InitGame();
	friend class Coral;

	friend class cRenderableWithShader;

	cRenderable *first;
	GLuint backBuffer;

	int postProcessEndLevel;

	cPriorityVector<cPostProcess*> postProcesses;
public:
	cGame();
	cShaderShr lastShader;
	RenderableAlignment lastAllignment;

	virtual void init() {}
	virtual void render() {}
	virtual void tick() {}
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

	void setSlowdown(float slowdown)
	{
		this->slowdown = slowdown;
	}

	float getSlowdown() const
	{
		return slowdown;
	}

	void resetToBackBuffer();
	void addPostProcess(cPostProcess *postProcess, int level);
	void removePostProcess(cPostProcess *postProcess);

	const Vec2& getCameraPos() const
	{
		return cameraPos;
	}

	float getCameraZoom() const
	{
		return cameraZoom;
	}

	float getCameraAngle() const
	{
		return cameraAngle;
	}
	virtual void windowResized(int width, int height);
	Vec2 getRelativeMousePos(const Vec2& mousePos, RenderableAlignment alignment) const;
};