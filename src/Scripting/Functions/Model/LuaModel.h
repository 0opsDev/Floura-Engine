#ifndef LUA_MODEL_H
#define LUA_MODEL_H

#include <unordered_set>
#include <vector>
#include <string>
#include "core/Main.h"
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#pragma comment(lib, "lua54.lib")

class luaModel
{
public:

	void DrawModel(sol::state& luaState);

};

#endif // SCRIPT_ENGINE_H