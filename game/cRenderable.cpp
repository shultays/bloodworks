#include "cRenderable.h"

GLuint quad = -1;

cRenderableGroup::cRenderableGroup(cGame *game, int initialCapacity /*= 2*/) : cRenderable(game)
{
	renderables.reserve(initialCapacity);
}

void cRenderableGroup::render()
{
	if (dirty)
	{
		dirty = false;

		for (auto& childData : renderables)
		{
			childData.child->setWorldMatrix(childData.localMatrix * worldMatrix);
		}
	}

	for (auto& childData : renderables)
	{
		childData.child->render();
	}
}

void cRenderableGroup::addRenderable(cRenderable *child, Mat3 localMatrix)
{
	ChildData childData;
	childData.child = child;
	childData.localMatrix = localMatrix;
	renderables.push_back(childData);
}

void cRenderable::setWorldMatrix(const Mat3& worldMatrix)
{
	this->worldMatrix = worldMatrix;
}
