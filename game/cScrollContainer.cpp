#include "cScrollContainer.h"
#include "cSlider.h"
#include "cGame.h"
#include "cGlobals.h"

cScrollContainer::cScrollContainer(cGame* game) : cRenderable(game)
{
	slider = new cSlider(game, true);
	slider->setValue(1.0f);
	crop = Rect(-100, -100, 100, 100);
	maxScroll = 100.0f;
	content = new cRenderableContainer(game);
}

cScrollContainer::~cScrollContainer()
{
	SAFE_DELETE(slider);
	SAFE_DELETE(content);
}

void cScrollContainer::render(bool isIdentity, const Mat3& mat, const Rect& crop)
{
	content->render(isIdentity, mat, this->crop);
	slider->render(false, isIdentity ? worldMatrix : worldMatrix * mat, crop);
}

void cScrollContainer::setAlignment(RenderableAlignment alignment)
{
	cRenderable::setAlignment(alignment);
	content->setAlignment(alignment);
	slider->setAlignment(alignment);
}

void cScrollContainer::setColor(const Vec4& color)
{
	cRenderable::setColor(color);
	content->setColor(color);
	slider->setColor(color);
}

void cScrollContainer::check(const Vec2& mousePos)
{
	slider->check(mousePos);
	if (slider->isChanged())
	{
		float val = 1.0f - slider->getFloatValue();
		content->setWorldMatrix(Mat3::translationMatrix(0.0f, val * (maxScroll - (crop.getMax().y - crop.getMin().y))));
	}
}

void cScrollContainer::addRenderable(cRenderable *child)
{
	content->addRenderable(child);
	child->setShader(resources.getShader("resources/defaultWithCrop"));
}

void cScrollContainer::setRect(const Rect& rect)
{
	this->crop = rect;
	Vec2 pos, size;
	pos.x = rect.getMax().x;
	pos.y = rect.getMid().y;
	slider->setWorldMatrix(Mat3::translationMatrix(pos));
	slider->setLength((rect.getMax().y - rect.getMin().y) * 0.5f);
}

void cScrollContainer::setMaxScroll(float maxScroll)
{
	this->maxScroll = maxScroll;
}

const Mat3& cScrollContainer::getScrollMatrix() const
{
	return content->getWorldMatrix();
}

bool cScrollContainer::isMouseInside(const Vec2& mousePos) const
{
	return crop.isInside(game->getRelativeMousePos(mousePos, alignment));
}
