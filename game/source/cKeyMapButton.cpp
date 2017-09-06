#include "cKeyMapButton.h"
#include "cButton.h"
#include "cFont.h"
#include "cTextRenderable.h"
#include "cTexturedQuadRenderable.h"
#include "cTexture.h"
#include "cGame.h"

cKeyMapButton::cKeyMapButton(cGame *game) : cRenderableContainer(game)
{
	bgButton = new cButton(game);
	cTexturedQuadRenderable *quad = new cTexturedQuadRenderable(game, "resources/ui/slider_bg.png", "resources/default");
	bgSize = quad->getTexture()->getDimensions().toVec();
	quad->setWorldMatrix(Mat3::identity());
	bgButton->addRenderable(quad);
	addRenderable(bgButton);
	bgButton->setDefaultMatrix(Vec2(0.0f), bgSize, 0.0f);
	bgButton->setHoverMatrix(Vec2(0.0f), bgSize, 0.0f);
	bgButton->setHitArea(-bgSize, bgSize);


	text = new cTextRenderable(game, resources.getFont("resources/fontData.txt"), "", 18.0f);
	text->setWorldMatrix(Mat3::translationMatrix(0.0f, bgSize.h * 0.7f));
	text->setTextAlignment(TextAlignment::center);
	text->setVerticalTextAlignment(VerticalTextAlignment::mid);
	addRenderable(text);

	changed = false;
	inUse = false;
	gameKey = key_invalid;
}

void cKeyMapButton::check(const Vec2& mousePos, bool ignoreClick)
{
	Vec2 shiftedPos = mousePos - getPosition() / game->getCameraZoom();
	bgButton->check(shiftedPos, ignoreClick);

	changed = false;
	if (inUse)
	{
		if (input.isKeyPressed(key_escape))
		{
			cancel();
		}
		else
		{
			for (int k = 0; k < key_count; k++)
			{
				if (k != key_escape && k != key_return && input.isKeyPressed(k))
				{
					text->setText(input.getKeyName((Key)k));
					gameKey = (Key)k;

					changed = true;
					inUse = false;
					bgButton->clearPress();
				}
			}
		}
	}
	else if (bgButton->isClicked())
	{
		setInUse();
	}
}

void cKeyMapButton::cancel()
{
	if (inUse)
	{
		inUse = false;
		text->setText(input.getKeyName(gameKey));
	}
}

void cKeyMapButton::mapKeysTo(MappedKey key) const
{
	mapper.setKeyMap(key, gameKey);
}

void cKeyMapButton::setKey(const Key gameKey)
{
	this->gameKey = gameKey;
	text->setText(input.getKeyName(gameKey));
}

void cKeyMapButton::setInUse()
{
	inUse = true;
	text->setText("Press Any Key");
}

