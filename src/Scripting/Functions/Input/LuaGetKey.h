#ifndef LUA_GET_KEY_H
#define LUA_GET_KEY_H

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#pragma comment(lib, "lua54.lib")
#include "utils/init.h"

class LuaGetKey
{
public:

	void ProcessKeyPresses(sol::state& luaState);

};

#endif // LUA_GET_KEY_H