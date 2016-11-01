#include "cRenderable.h"

GLuint cTexturedQuadRenderable::quad = -1;

void cTexturedQuadRenderable::setWorldFrame(const Mat3& mat)
{
	worldMatrix = mat;
}
