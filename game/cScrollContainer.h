#pragma once

#include "cRenderable.h"
class cSlider;

class cScrollContainer : public cRenderable
{
	cSlider* slider;
	cRenderableGroup* content;
	Rect crop;
	float maxScroll;
public:
	cScrollContainer(cGame* game);
	~cScrollContainer();

	virtual void render(bool isIdentity, const Mat3& mat, const Rect& crop) override;
	virtual void setAlignment(RenderableAlignment alignment) override;

	virtual void setColor(const Vec4& color) override;
	void check(const Vec2& mousePos);

	void addRenderable(cRenderable *child);
	void setRect(const Rect& rect);
	void setMaxScroll(float maxScroll);
	const Mat3& getScrollMatrix() const;
	const Rect& getRect() const 
	{
		return crop;
	}
	bool isMouseInside(const Vec2& mousePos) const;
};