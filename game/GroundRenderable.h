#pragma once

#include "cRenderableContainer.h"

class Bloodworks;

class GroundRenderable : public cRenderable
{
	GLuint frameBuffer;
	GLuint frameBufferTexture;

	Bloodworks *bloodworks;
public:
	GroundRenderable(Bloodworks *bloodworks);
	virtual ~GroundRenderable();
	virtual void render(bool isIdentity, const Mat3& mat, const AARect& crop) override;
};