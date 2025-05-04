#ifndef LUA_CAMERA_H
#define LUA_CAMERA_H

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#pragma comment(lib, "lua54.lib")
#include "utils/init.h"

class LuaCamera
{
public:

	void SetCameraPos(sol::state& luaState);

};

#endif // LUA_CAMERA_H