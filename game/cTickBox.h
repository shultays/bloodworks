#pragma once

#include "cButton.h"

class cTickbox : public cButton
{
	cTexturedQuadRenderable *checkedRenderable;
	cTexturedQuadRenderable *uncheckedRenderable;
	bool checked;
public:
	cTickbox(cGame *game, const std::string &checked = "resources/ui/checked_box.png", const std::string &unchecked = "resources/ui/unchecked_box.png", bool isChecked = false);
	void setChecked(bool isChecked);
	void check(const Vec2& mousePos);
	bool isChecked() const
	{
		return checked;
	}
};