#include "cTickBox.h"
#include "cTexturedQuadRenderable.h"
#include "cTexture.h"

cTickBox::cTickBox(cGame *game, const std::string &checked, const std::string &unchecked, bool isChecked /*= false*/) : cButton(game)
{
	checkedRenderable = new cTexturedQuadRenderable(game, checked, "resources/default");
	checkedRenderable->setWorldMatrix(Mat3::identity());
	checkedRenderable->setAlignment(getAlignment());
	addRenderable(checkedRenderable);

	uncheckedRenderable = new cTexturedQuadRenderable(game, unchecked, "resources/default");
	uncheckedRenderable->setWorldMatrix(Mat3::identity());
	uncheckedRenderable->setAlignment(getAlignment());
	addRenderable(uncheckedRenderable);

	setChecked(isChecked);
}

void cTickBox::setChecked(bool isChecked)
{
	this->checked = isChecked;

	checkedRenderable->setVisible(isChecked);
	uncheckedRenderable->setVisible(!isChecked);
}

void cTickBox::check(const Vec2& mousePos, bool ignoreClick)
{
	cButton::check(mousePos);
	if (isClicked())
	{
		setChecked(!checked);
	}
}

