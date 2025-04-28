#ifndef LUA_SHADER_H
#define LUA_SHADER_H

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#pragma comment(lib, "lua54.lib")
#include "Systems/utils/init.h"

class luaShader
{
public:

	void SetShader(sol::state& luaState);

};

#endif // SCRIPT_ENGINE_H