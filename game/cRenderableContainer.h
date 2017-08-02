#pragma once

#include "cRenderable.h"
#include <vector>

class cRenderableContainer : public cRenderable
{
	struct ChildData
	{
		cRenderable *child;
	};
	std::vector<ChildData> renderables;

public:
	cRenderableContainer(cGame *game, int initialCapacity = 2);
	~cRenderableContainer()
	{
		for (auto& childData : renderables)
		{
			SAFE_DELETE(childData.child);
		}
	}

	void addRenderable(cRenderable *child);
	void removeRenderable(cRenderable *child);

	virtual void render(bool isIdentity, const Mat3& mat, const AARect& crop) override;
	virtual void setAlignment(RenderableAlignment alignment) override
	{
		cRenderable::setAlignment(alignment);
		for (auto& childData : renderables)
		{
			childData.child->setAlignment(alignment);
		}
	}

	virtual void setColor(const Vec4& color) override
	{
		cRenderable::setColor(color);

		for (auto& childData : renderables)
		{
			childData.child->setColor(color);
		}
	}

	virtual void setShader(const cShaderShr& shader) override
	{
		cRenderable::setShader(shader);

		for (auto& childData : renderables)
		{
			childData.child->setShader(shader);
		}
	}
};
