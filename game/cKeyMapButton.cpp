#include "cKeyMapButton.h"
#include "cButton.h"
#include "cFont.h"
#include "cTextRenderable.h"
#include "cTexturedQuadRenderable.h"
#include "cTexture.h"

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
	keyCount = false;
}

void cKeyMapButton::check(const Vec2& mousePos, bool ignoreClick)
{
	Vec2 shiftedPos = mousePos - getPosition();
	bgButton->check(shiftedPos, ignoreClick);

	changed = false;
	if (inUse)
	{
		if (input.isKeyPressed(key_escape))
		{
			cancel();
		}
		else if (input.isKeyPressed(key_return) || keyCount == 4)
		{
			if (keys[0] == key_invalid)
			{
				cancel();
			}
			else
			{
				changed = true;
				inUse = false;
			}
		}
		else
		{
			for (int k = 0; k < key_count; k++)
			{
				if (k != key_escape && k != key_return && input.isKeyPressed(k))
				{
					if (keys[0] != k && keys[1] != k && keys[2] != k && keys[3] != k)
					{
						if (keyCount == 0)
						{
							text->setText(input.getKeyName((Key)k));
						}
						else
						{
							text->setText(text->getText() + " " + input.getKeyName((Key)k));
						}
						keys[keyCount++] = (Key)k;
					}
				}
			}
		}
	}
	else if (bgButton->isClicked())
	{
		keyCount = 0;
		keys[0] = key_invalid;
		keys[1] = key_invalid;
		keys[2] = key_invalid;
		keys[3] = key_invalid;
		inUse = true;
		oldText = text->getText();
		text->setText("");
	}

}

void cKeyMapButton::cancel()
{
	if (inUse)
	{
		inUse = false;
		text->setText(oldText);
	}
}

void cKeyMapButton::mapKeysTo(MappedKey key) const
{
	if (keyCount)
	{
		mapper.setKeyMap(key, keys[0], keys[1], keys[2], keys[3]);
	}
}

void cKeyMapButton::setKeys(const Key keys[4])
{
	if (keys[0] == key_invalid)
	{
		keyCount = 0;
		text->setText("Unassigned");
	}
	else
	{
		keyCount = 1;
		text->setText(input.getKeyName(keys[0]));
		this->keys[0] = keys[0];
		for (int i = 1; i < 4; i++)
		{
			if (keys[i] == key_invalid)
			{
				break;
			}
			else
			{
				this->keys[i] = keys[i];
				text->setText(text->getText() + " " + input.getKeyName(keys[i]));
			}
		}
	}
}

