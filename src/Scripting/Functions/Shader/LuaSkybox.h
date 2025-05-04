#ifndef LUA_SKYBOX_H
#define LUA_SKYBOX_H

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#pragma comment(lib, "lua54.lib")
#include "utils/init.h"

class luaSkybox
{
public:

	void SetSkybox(sol::state& luaState);

};

#endif // LUA_SKYBOX_H