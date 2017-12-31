#include "cInput.h"
#include "cGlobals.h"
#include <stdio.h>
#include <string.h>
#include "SDL.h" 

extern SDL_Window *mainWindow;

void cInput::init()
{
	memset(keyStates, 0, sizeof(keyStates));
	memset(prevKeyStates, 0, sizeof(prevKeyStates));
	memset(keyTime, 0, sizeof(keyTime));
	ignoreNextMove = true;
	hideMouse();
	prevMousePos = mousePos = Vec2::zero();
	joyAxisPos = Vec2::zero();
	joyAxisPos2 = Vec2::zero();
	mouseWheel.setZero();
	setLuaKeys();

	useEvents = true;
}

void cInput::pressKey(int key)
{
	keyStates[key] = true;
	keyTime[key] = timer.getTime();
}

void cInput::tick()
{
	memcpy(prevKeyStates, keyStates, sizeof(prevKeyStates));

	prevMousePos = mousePos;
	mouseWheel.setZero();
}

void cInput::preTick()
{
	if (useEvents == false)
	{
		int x, y;

		if (mouseShown)
		{
			SDL_GetMouseState(&x, &y);
			mousePos = Vec2((float)x, (float)(coral.getWinodwHeight() - y));
		}
		else
		{
			SDL_GetRelativeMouseState(&x, &y);
			prevMousePos -= Vec2((float)x, (float)-y);
		}
		if (ignoreNextMove)
		{
			ignoreNextMove = false;
			prevMousePos = mousePos;
		}
	}
}

void cInput::showMouse()
{
	ignoreNextMove = true;
	prevMousePos = mousePos;
	mouseShown = true;
	SDL_SetRelativeMouseMode(SDL_FALSE);
	SDL_ShowCursor(SDL_TRUE);

	if (useEvents == false)
	{
		int x, y;
		SDL_GetMouseState(&x, &y);
		SDL_GetRelativeMouseState(&x, &y);
	}
}

void cInput::hideMouse()
{
	ignoreNextMove = true;
	prevMousePos = mousePos;
	mouseShown = false;
	SDL_SetRelativeMouseMode(SDL_TRUE);
	SDL_ShowCursor(SDL_FALSE);

	if (useEvents == false)
	{
		int x, y;
		SDL_GetMouseState(&x, &y);
		SDL_GetRelativeMouseState(&x, &y);
	}
}

bool cInput::hasJoyStick() const
{
	return SDL_NumJoysticks() > 0;
}

void cInput::setMousePosition(int x, int y)
{
	SDL_WarpMouseInWindow(mainWindow, x, y);
}

void cInput::setJoystickAxis(int joystick, int axis, int value)
{
	if (axis < 6)
	{
		float val = fabs(value / 32767.0f);
		val *= 1.2f;
		val -= 0.1f;
		clamp(val, 0.0f, 1.0f);
		if (value < 0)
		{
			val = -val;
		}
		if (axis%3 == 1)
		{
			val = -val;
		}
		if (axis < 2)
		{
			joyAxisPos[axis] = val;
		}
		else if (axis >= 3 && axis <=4)
		{
			joyAxisPos2[axis - 3] = val;
		}
	}
}

const std::string& cInput::getKeyName(Key key)
{
	return names[key];
}

void cInput::clearWheel()
{
	mouseWheel.setZero();
}

void cInput::setUseEvents(bool useEvents)
{
	this->useEvents = useEvents;
	int x, y;
	SDL_GetMouseState(&x, &y);
	SDL_GetRelativeMouseState(&x, &y);
}

void cInput::mouseWhellMove(int x, int y)
{
	mouseWheel.x += x;
	mouseWheel.y += y;
}

void cInput::releaseKey(int key)
{
	keyStates[key] = 0;
	keyTime[key] = timer.getTime();
}

void cInput::setMousePos(const Vec2& pos, const Vec2& relativePos)
{
	if (useEvents)
	{
		if (mouseShown)
		{
			mousePos = Vec2(pos.x, pos.y);
		}
		else
		{
			prevMousePos -= relativePos;
		}
		if (ignoreNextMove)
		{
			ignoreNextMove = false;
			prevMousePos = mousePos;
		}
	}
}

void cInput::setLuaKeys()
{
	nameMap["Unassigned"] = key_invalid;
	nameMap["A"] = key_a;
	nameMap["B"] = key_b;
	nameMap["C"] = key_c;
	nameMap["D"] = key_d;
	nameMap["E"] = key_e;
	nameMap["F"] = key_f;
	nameMap["G"] = key_g;
	nameMap["H"] = key_h;
	nameMap["I"] = key_i;
	nameMap["J"] = key_j;
	nameMap["K"] = key_k;
	nameMap["L"] = key_l;
	nameMap["M"] = key_m;
	nameMap["N"] = key_n;
	nameMap["O"] = key_o;
	nameMap["P"] = key_p;
	nameMap["Q"] = key_q;
	nameMap["R"] = key_r;
	nameMap["S"] = key_s;
	nameMap["T"] = key_t;
	nameMap["U"] = key_u;
	nameMap["V"] = key_v;
	nameMap["W"] = key_w;
	nameMap["X"] = key_x;
	nameMap["Y"] = key_y;
	nameMap["Z"] = key_z;
	nameMap["1"] = key_1;
	nameMap["2"] = key_2;
	nameMap["3"] = key_3;
	nameMap["4"] = key_4;
	nameMap["5"] = key_5;
	nameMap["6"] = key_6;
	nameMap["7"] = key_7;
	nameMap["8"] = key_8;
	nameMap["9"] = key_9;
	nameMap["0"] = key_0;
	nameMap["Return"] = key_return;
	nameMap["Escape"] = key_escape;
	nameMap["Backspace"] = key_backspace;
	nameMap["Tab"] = key_tab;
	nameMap["Space"] = key_space;
	nameMap["-"] = key_minus;
	nameMap["="] = key_equals;
	nameMap["["] = key_leftbracket;
	nameMap["]"] = key_rightbracket;
	nameMap["\\"] = key_backslash;
	nameMap["nonushash"] = key_nonushash;
	nameMap[";"] = key_semicolon;
	nameMap["'"] = key_apostrophe;
	nameMap["`"] = key_grave;
	nameMap["."] = key_comma;
	nameMap["."] = key_period;
	nameMap["/"] = key_slash;
	nameMap["Capslock"] = key_capslock;
	nameMap["F1"] = key_f1;
	nameMap["F2"] = key_f2;
	nameMap["F3"] = key_f3;
	nameMap["F4"] = key_f4;
	nameMap["F5"] = key_f5;
	nameMap["F6"] = key_f6;
	nameMap["F7"] = key_f7;
	nameMap["F8"] = key_f8;
	nameMap["F9"] = key_f9;
	nameMap["F10"] = key_f10;
	nameMap["F11"] = key_f11;
	nameMap["F12"] = key_f12;
	nameMap["PrintScr"] = key_printscreen;
	nameMap["ScrLck"] = key_scrolllock;
	nameMap["Pause"] = key_pause;
	nameMap["Insert"] = key_insert;
	nameMap["Home"] = key_home;
	nameMap["PageUp"] = key_pageup;
	nameMap["Delete"] = key_delete;
	nameMap["End"] = key_end;
	nameMap["PageDown"] = key_pagedown;
	nameMap["Right"] = key_right;
	nameMap["Left"] = key_left;
	nameMap["Down"] = key_down;
	nameMap["Up"] = key_up;
	nameMap["Clear"] = key_numlockclear;
	nameMap["NumDivide"] = key_num_divide;
	nameMap["NumMultiply"] = key_num_multiply;
	nameMap["NumMinus"] = key_num_minus;
	nameMap["NumPlus"] = key_num_plus;
	nameMap["NumEnter"] = key_num_enter;
	nameMap["Num1"] = key_num_1;
	nameMap["Num2"] = key_num_2;
	nameMap["Num3"] = key_num_3;
	nameMap["Num4"] = key_num_4;
	nameMap["Num5"] = key_num_5;
	nameMap["Num6"] = key_num_6;
	nameMap["Num7"] = key_num_7;
	nameMap["Num8"] = key_num_8;
	nameMap["Num9"] = key_num_9;
	nameMap["Num0"] = key_num_0;
	nameMap["NumPeriod"] = key_num_period;
	nameMap["NumBackSlash"] = key_nonusbackslash;
	nameMap["Application"] = key_application;
	nameMap["Power"] = key_power;
	nameMap["NumEquals"] = key_num_equals;
	nameMap["F13"] = key_f13;
	nameMap["F14"] = key_f14;
	nameMap["F15"] = key_f15;
	nameMap["F16"] = key_f16;
	nameMap["F17"] = key_f17;
	nameMap["F18"] = key_f18;
	nameMap["F19"] = key_f19;
	nameMap["F20"] = key_f20;
	nameMap["F21"] = key_f21;
	nameMap["F22"] = key_f22;
	nameMap["F23"] = key_f23;
	nameMap["F24"] = key_f24;
	nameMap["execute"] = key_execute;
	nameMap["help"] = key_help;
	nameMap["menu"] = key_menu;
	nameMap["select"] = key_select;
	nameMap["stop"] = key_stop;
	nameMap["again"] = key_again;
	nameMap["undo"] = key_undo;
	nameMap["cut"] = key_cut;
	nameMap["copy"] = key_copy;
	nameMap["paste"] = key_paste;
	nameMap["find"] = key_find;
	nameMap["mute"] = key_mute;
	nameMap["volumeup"] = key_volumeup;
	nameMap["volumedown"] = key_volumedown;
	nameMap["NumComma"] = key_num_comma;
	nameMap["num_equalsas400"] = key_num_equalsas400;
	nameMap["international1"] = key_international1;
	nameMap["international2"] = key_international2;
	nameMap["international3"] = key_international3;
	nameMap["international4"] = key_international4;
	nameMap["international5"] = key_international5;
	nameMap["international6"] = key_international6;
	nameMap["international7"] = key_international7;
	nameMap["international8"] = key_international8;
	nameMap["international9"] = key_international9;
	nameMap["lang1"] = key_lang1;
	nameMap["lang2"] = key_lang2;
	nameMap["lang3"] = key_lang3;
	nameMap["lang4"] = key_lang4;
	nameMap["lang5"] = key_lang5;
	nameMap["lang6"] = key_lang6;
	nameMap["lang7"] = key_lang7;
	nameMap["lang8"] = key_lang8;
	nameMap["lang9"] = key_lang9;
	nameMap["alterase"] = key_alterase;
	nameMap["sysreq"] = key_sysreq;
	nameMap["cancel"] = key_cancel;
	nameMap["clear"] = key_clear;
	nameMap["prior"] = key_prior;
	nameMap["return2"] = key_return2;
	nameMap["separator"] = key_separator;
	nameMap["out"] = key_out;
	nameMap["oper"] = key_oper;
	nameMap["clearagain"] = key_clearagain;
	nameMap["crsel"] = key_crsel;
	nameMap["exsel"] = key_exsel;
	nameMap["num_00"] = key_num_00;
	nameMap["num_000"] = key_num_000;
	nameMap["thousandsseparator"] = key_thousandsseparator;
	nameMap["decimalseparator"] = key_decimalseparator;
	nameMap["currencyunit"] = key_currencyunit;
	nameMap["currencysubunit"] = key_currencysubunit;
	nameMap["num_leftparen"] = key_num_leftparen;
	nameMap["num_rightparen"] = key_num_rightparen;
	nameMap["num_leftbrace"] = key_num_leftbrace;
	nameMap["num_rightbrace"] = key_num_rightbrace;
	nameMap["num_tab"] = key_num_tab;
	nameMap["num_backspace"] = key_num_backspace;
	nameMap["num_a"] = key_num_a;
	nameMap["num_b"] = key_num_b;
	nameMap["num_c"] = key_num_c;
	nameMap["num_d"] = key_num_d;
	nameMap["num_e"] = key_num_e;
	nameMap["num_f"] = key_num_f;
	nameMap["num_xor"] = key_num_xor;
	nameMap["num_power"] = key_num_power;
	nameMap["num_percent"] = key_num_percent;
	nameMap["num_less"] = key_num_less;
	nameMap["num_greater"] = key_num_greater;
	nameMap["num_ampersand"] = key_num_ampersand;
	nameMap["num_dblampersand"] = key_num_dblampersand;
	nameMap["num_verticalbar"] = key_num_verticalbar;
	nameMap["num_dblverticalbar"] = key_num_dblverticalbar;
	nameMap["num_colon"] = key_num_colon;
	nameMap["num_hash"] = key_num_hash;
	nameMap["num_space"] = key_num_space;
	nameMap["num_at"] = key_num_at;
	nameMap["num_exclam"] = key_num_exclam;
	nameMap["num_memstore"] = key_num_memstore;
	nameMap["num_memrecall"] = key_num_memrecall;
	nameMap["num_memclear"] = key_num_memclear;
	nameMap["num_memadd"] = key_num_memadd;
	nameMap["num_memsubtract"] = key_num_memsubtract;
	nameMap["num_memmultiply"] = key_num_memmultiply;
	nameMap["num_memdivide"] = key_num_memdivide;
	nameMap["num_plusminus"] = key_num_plusminus;
	nameMap["num_clear"] = key_num_clear;
	nameMap["num_clearentry"] = key_num_clearentry;
	nameMap["num_binary"] = key_num_binary;
	nameMap["num_octal"] = key_num_octal;
	nameMap["num_decimal"] = key_num_decimal;
	nameMap["num_hexadecimal"] = key_num_hexadecimal;
	nameMap["lctrl"] = key_lctrl;
	nameMap["lshift"] = key_lshift;
	nameMap["lalt"] = key_lalt;
	nameMap["lgui"] = key_lgui;
	nameMap["rctrl"] = key_rctrl;
	nameMap["rshift"] = key_rshift;
	nameMap["ralt"] = key_ralt;
	nameMap["rgui"] = key_rgui;
	nameMap["mode"] = key_mode;
	nameMap["audionext"] = key_audionext;
	nameMap["audioprev"] = key_audioprev;
	nameMap["audiostop"] = key_audiostop;
	nameMap["audioplay"] = key_audioplay;
	nameMap["audiomute"] = key_audiomute;
	nameMap["mediaselect"] = key_mediaselect;
	nameMap["www"] = key_www;
	nameMap["mail"] = key_mail;
	nameMap["calculator"] = key_calculator;
	nameMap["computer"] = key_computer;
	nameMap["ac_search"] = key_ac_search;
	nameMap["ac_home"] = key_ac_home;
	nameMap["ac_back"] = key_ac_back;
	nameMap["ac_forward"] = key_ac_forward;
	nameMap["ac_stop"] = key_ac_stop;
	nameMap["ac_refresh"] = key_ac_refresh;
	nameMap["ac_bookmarks"] = key_ac_bookmarks;
	nameMap["brightnessdown"] = key_brightnessdown;
	nameMap["brightnessup"] = key_brightnessup;
	nameMap["displayswitch"] = key_displayswitch;
	nameMap["kbdillumtoggle"] = key_kbdillumtoggle;
	nameMap["kbdillumdown"] = key_kbdillumdown;
	nameMap["kbdillumup"] = key_kbdillumup;
	nameMap["eject"] = key_eject;
	nameMap["sleep"] = key_sleep;
	nameMap["app1"] = key_app1;
	nameMap["app2"] = key_app2;
	nameMap["LeftMouse"] = mouse_button_left;
	nameMap["MiddleMouse"] = mouse_button_middle;
	nameMap["RightMouse"] = mouse_button_right;
	nameMap["Mouse4"] = mouse_button_4;
	nameMap["Mouse5"] = mouse_button_5;

	nameMap["j_up"] = joystick_0_button_up;
	nameMap["j_down"] = joystick_0_button_down;
	nameMap["j_left"] = joystick_0_button_left;
	nameMap["j_right"] = joystick_0_button_right;

	nameMap["j_a"] = joystick_0_button_a;
	nameMap["j_b"] = joystick_0_button_b;
	nameMap["j_x"] = joystick_0_button_x;
	nameMap["j_y"] = joystick_0_button_y;
	nameMap["j_leftshoulder"] = joystick_0_button_leftshoulder;
	nameMap["j_rightshoulder"] = joystick_0_button_rightshoulder;
	nameMap["j_back"] = joystick_0_button_back;
	nameMap["j_start"] = joystick_0_button_start;

	auto keys = lua["keys"] = lua.create_table();
	for (auto& key : nameMap)
	{
		names[key.second] = key.first;
		keys[key.first] = key.second;
	}
}
