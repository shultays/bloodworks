#pragma once

#include "cInput.h"
#include "KeyMapper.h"
#include "cTime.h"
#include "Coral.h"
#include "cResources.h"
#include "cDebugRenderable.h"
#include "cLuaWorld.h"

class cGame;

extern cInput input;
extern cTime timer;
extern Coral coral;
extern cResources resources;
extern cDebugRenderable debugRenderer;
extern cGame *game;
extern cLuaWorld lua;
extern KeyMapper mapper;