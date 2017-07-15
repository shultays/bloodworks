#include "cRenderableContainer.h"

cRenderableContainer::cRenderableContainer(cGame *game, int initialCapacity /*= 2*/) : cRenderable(game)
{
	renderables.reserve(initialCapacity);
}

void cRenderableContainer::render(bool isIdentity, const Mat3& mat, const Rect& crop)
{
	for (auto& childData : renderables)
	{
		if (childData.child->isVisible())
		{
			childData.child->render(false, isIdentity ? worldMatrix : worldMatrix * mat, crop);
		}
	}
}

void cRenderableContainer::addRenderable(cRenderable *child)
{
	ChildData childData;
	childData.child = child;
	child->setAlignment(alignment);
	renderables.push_back(childData);
}
