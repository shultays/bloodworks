#pragma once

#include "cRenderableContainer.h"

class cButton;
class cTextRenderable;
enum Key;
typedef int MappedKey;

class cKeyMapButton : public cRenderableContainer
{
	cButton *bgButton;
	cTextRenderable *text;

	bool changed;
	bool inUse;
	Vec2 bgSize;
	
	Key gameKey;
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
	void setKey(const Key gameKey);
	void setInUse();
};