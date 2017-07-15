#pragma once

#include "cRenderable.h"

class cButton;
class cTextRenderable;

class cKeyMapButton : public cRenderableGroup
{
	cButton *bgButton;
	cTextRenderable *text;

	bool changed;
	bool inUse;
	Vec2 bgSize;
	std::string oldText;
	
	int keyCount;
	Key keys[4];
public:
	cKeyMapButton(cGame *game);
	void check(const Vec2& mousePos, bool ignoreClick = false);
	bool isChanged() const
	{
		return changed;
	}
	bool isInUse() const
	{
		return inUse;
	}
	void cancel();
	void mapKeysTo(MappedKey key) const;
	void setKeys(const Key keys[4]);
};