#pragma once

#include "cRenderableContainer.h"

class Bloodworks;

class GroundRenderable : public cRenderable
{
	struct GroundBuffer
	{
		int xx;
		int yy;
		GLuint frameBuffer;
		GLuint frameBufferTexture;
	};
	cVector<GroundBuffer> groundBuffers;


	Bloodworks *bloodworks;
	int groundSize;
public:
	GroundRenderable(Bloodworks *bloodworks);
	virtual ~GroundRenderable();
	virtual void render(bool isIdentity, const Mat3& mat, const AARect& crop) override;
};