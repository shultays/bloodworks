#include "BloodworksControls.h"
#include "cGlobals.h"

void BloodworksControls::init()
{
	const Key GameKeyValues[][4] =
	{
		{ key_left, key_a },
		{ key_right, key_d },
		{ key_up, key_w },
		{ key_down, key_s },

		{ mouse_button_left, joystick_0_button_rightshoulder },
		{ mouse_button_right, joystick_0_button_a },
		{ mouse_button_middle, joystick_0_button_b },

		{ key_r, mouse_button_4, joystick_0_button_leftshoulder },
		{ key_tab, joystick_0_button_y },
		{ key_e, mouse_button_5, joystick_0_button_x },

		{ key_p, joystick_0_button_start },
		{ key_space },

		{ key_return, key_space, joystick_0_button_x },
		{ key_escape, joystick_0_button_back },
		{ key_f10 },

	};

	const char* GameKeyNames[] =
	{
		"Left",
		"Right",
		"Up",
		"Down",
		"Attack",
		"Attack2",
		"Special",
		"Reload",
		"Level_Up",
		"Pause",
		"Swap_Guns",
		"Show_Hints",
		"Select",
		"Back",
		"Toggle_Fullscreen",
	};

	mapper.setSavePath("keys.txt");
	for (int i = 0; i < (int)GameKey::EndStatic; i++)
	{
		GameKey key = (GameKey)i;
		mapper.addKeyMap(GameKeyNames[i], GameKeyValues[i][0], GameKeyValues[i][1], GameKeyValues[i][2], GameKeyValues[i][3]);
	}
}
