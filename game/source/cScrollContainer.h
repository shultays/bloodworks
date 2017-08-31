#pragma once

#include "cRenderable.h"
class cSlider;

class cScrollContainer : public cRenderable
{
	cSlider* slider;
	cRenderableContainer* content;
	AARect crop;
	float maxScroll;
public:
	cScrollContainer(cGame* game);
	~cScrollContainer();

	virtual void render(bool isIdentity, const Mat3& mat, const AARect& crop) override;
	virtual void setAlignment(RenderableAlignment alignment) override;

	virtual void setColor(const Vec4& color) override;
	void check(const Vec2& mousePos);

	void addRenderable(cRenderable *child);
	void removeRenderable(cRenderable *child);

	void setRect(const AARect& rect);
	void setMaxScroll(float maxScroll);
	const Mat3& getScrollMatrix() const;
	const AARect& getRect() const 
	{
		return crop;
	}
	bool isMouseInside(const Vec2& mousePos) const;
	void setScroll(float scroll);
};