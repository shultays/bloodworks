#include "BloodworksControls.h"
#include "cGlobals.h"


cVector<struct BloodworksControls::KeyData> BloodworksControls::keys;

void BloodworksControls::init()
{
	const Key GameKeyValues[][4] =
	{
		{ key_w,					joystick_0_button_up },
		{ key_s,					joystick_0_button_down },

		{ key_a,					joystick_0_button_left },
		{ key_d,					joystick_0_button_right },

		{ mouse_button_left,		joystick_0_button_rightshoulder },
		{ mouse_button_right,		joystick_0_button_a },

		{ key_r,					joystick_0_button_leftshoulder },
		{ key_tab,					joystick_0_button_y },

		{ key_return,				joystick_0_button_x },
		{ key_escape,				joystick_0_button_back },
		{ key_f10,					key_invalid},

	};

	const char* GameKeyNames[] =
	{
		"Up",
		"Down",
		"Left",
		"Right",
		"Attack",
		"Ultimate",
		"Reload",
		"Level_Up",
		"Select",
		"Back",
		"Toggle_Fullscreen",
	};

	mapper.setSavePath( (int)GameKey::Count, GameKeyNames, "keys.txt");
	for (int i = 0; i < (int)GameKey::Count; i++)
	{
		BloodworksControls::KeyData keyData;
		GameKey key = (GameKey)i;
		keyData.key = mapper.addKeyMap(GameKeyNames[i], GameKeyValues[i][0], GameKeyValues[i][1], GameKeyValues[i][2], GameKeyValues[i][3]);

		keyData.keyName = GameKeyNames[i];
		for (int i = 0; i < keyData.keyName.size(); i++)
		{
			if (keyData.keyName[i] == '_')
			{
				keyData.keyName[i] = ' ';
			}
		}
		keyData.defaults[0] = GameKeyValues[i][0];
		keyData.defaults[1] = GameKeyValues[i][1];
		keyData.defaults[2] = GameKeyValues[i][2];
		keyData.defaults[3] = GameKeyValues[i][3];
		keys.push_back(keyData);
	}
}
