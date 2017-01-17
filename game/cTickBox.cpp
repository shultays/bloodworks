#include "cTickBox.h"
#include "cTexture.h"

cTickbox::cTickbox(cGame *game, const std::string &checked, const std::string &unchecked, bool isChecked /*= false*/) : cButton(game)
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

void cTickbox::setChecked(bool isChecked)
{
	this->checked = isChecked;

	checkedRenderable->setVisible(isChecked);
	uncheckedRenderable->setVisible(!isChecked);
}

void cTickbox::check(const Vec2& mousePos)
{
	cButton::check(mousePos);
	if (isClicked())
	{
		setChecked(!checked);
	}
}

