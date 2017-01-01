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


	setLuaKeys();
}

void cInput::pressKey(int key)
{
	keyStates[key] = 1;
	keyTime[key] = timer.getTime();
}

void cInput::tick()
{
	memcpy(prevKeyStates, keyStates, sizeof(prevKeyStates));
	prevMousePos = mousePos;
}

void cInput::setMousePosition(int x, int y)
{
	SDL_WarpMouseInWindow(mainWindow, x, y);
}

void cInput::releaseKey(int key)
{
	keyStates[key] = 0;
	keyTime[key] = timer.getTime();
}

void cInput::setMousePos(const Vec2& pos, const Vec2& relativePos)
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

void cInput::setLuaKeys()
{
	nameMap["a"] = key_a;
	nameMap["b"] = key_b;
	nameMap["c"] = key_c;
	nameMap["d"] = key_d;
	nameMap["e"] = key_e;
	nameMap["f"] = key_f;
	nameMap["g"] = key_g;
	nameMap["h"] = key_h;
	nameMap["i"] = key_i;
	nameMap["j"] = key_j;
	nameMap["k"] = key_k;
	nameMap["l"] = key_l;
	nameMap["m"] = key_m;
	nameMap["n"] = key_n;
	nameMap["o"] = key_o;
	nameMap["p"] = key_p;
	nameMap["q"] = key_q;
	nameMap["r"] = key_r;
	nameMap["s"] = key_s;
	nameMap["t"] = key_t;
	nameMap["u"] = key_u;
	nameMap["v"] = key_v;
	nameMap["w"] = key_w;
	nameMap["x"] = key_x;
	nameMap["y"] = key_y;
	nameMap["z"] = key_z;
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
	nameMap["return"] = key_return;
	nameMap["escape"] = key_escape;
	nameMap["backspace"] = key_backspace;
	nameMap["tab"] = key_tab;
	nameMap["space"] = key_space;
	nameMap["minus"] = key_minus;
	nameMap["equals"] = key_equals;
	nameMap["leftbracket"] = key_leftbracket;
	nameMap["rightbracket"] = key_rightbracket;
	nameMap["backslash"] = key_backslash;
	nameMap["nonushash"] = key_nonushash;
	nameMap["semicolon"] = key_semicolon;
	nameMap["apostrophe"] = key_apostrophe;
	nameMap["grave"] = key_grave;
	nameMap["comma"] = key_comma;
	nameMap["period"] = key_period;
	nameMap["slash"] = key_slash;
	nameMap["capslock"] = key_capslock;
	nameMap["f1"] = key_f1;
	nameMap["f2"] = key_f2;
	nameMap["f3"] = key_f3;
	nameMap["f4"] = key_f4;
	nameMap["f5"] = key_f5;
	nameMap["f6"] = key_f6;
	nameMap["f7"] = key_f7;
	nameMap["f8"] = key_f8;
	nameMap["f9"] = key_f9;
	nameMap["f10"] = key_f10;
	nameMap["f11"] = key_f11;
	nameMap["f12"] = key_f12;
	nameMap["printscreen"] = key_printscreen;
	nameMap["scrolllock"] = key_scrolllock;
	nameMap["pause"] = key_pause;
	nameMap["insert"] = key_insert;
	nameMap["home"] = key_home;
	nameMap["pageup"] = key_pageup;
	nameMap["delete"] = key_delete;
	nameMap["end"] = key_end;
	nameMap["pagedown"] = key_pagedown;
	nameMap["right"] = key_right;
	nameMap["left"] = key_left;
	nameMap["down"] = key_down;
	nameMap["up"] = key_up;
	nameMap["numlockclear"] = key_numlockclear;
	nameMap["num_divide"] = key_num_divide;
	nameMap["num_multiply"] = key_num_multiply;
	nameMap["num_minus"] = key_num_minus;
	nameMap["num_plus"] = key_num_plus;
	nameMap["num_enter"] = key_num_enter;
	nameMap["num_1"] = key_num_1;
	nameMap["num_2"] = key_num_2;
	nameMap["num_3"] = key_num_3;
	nameMap["num_4"] = key_num_4;
	nameMap["num_5"] = key_num_5;
	nameMap["num_6"] = key_num_6;
	nameMap["num_7"] = key_num_7;
	nameMap["num_8"] = key_num_8;
	nameMap["num_9"] = key_num_9;
	nameMap["num_0"] = key_num_0;
	nameMap["num_period"] = key_num_period;
	nameMap["nonusbackslash"] = key_nonusbackslash;
	nameMap["application"] = key_application;
	nameMap["power"] = key_power;
	nameMap["num_equals"] = key_num_equals;
	nameMap["f13"] = key_f13;
	nameMap["f14"] = key_f14;
	nameMap["f15"] = key_f15;
	nameMap["f16"] = key_f16;
	nameMap["f17"] = key_f17;
	nameMap["f18"] = key_f18;
	nameMap["f19"] = key_f19;
	nameMap["f20"] = key_f20;
	nameMap["f21"] = key_f21;
	nameMap["f22"] = key_f22;
	nameMap["f23"] = key_f23;
	nameMap["f24"] = key_f24;
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
	nameMap["num_comma"] = key_num_comma;
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
	nameMap["mouse_button_left"] = mouse_button_left;
	nameMap["mouse_button_middle"] = mouse_button_middle;
	nameMap["mouse_button_right"] = mouse_button_right;
	nameMap["mouse_button_4"] = mouse_button_4;
	nameMap["mouse_button_5"] = mouse_button_5;

	auto keys = lua["keys"] = lua.create_table();
	for (auto& key : nameMap)
	{
		keys[key.first] = key.second;
	}
}
