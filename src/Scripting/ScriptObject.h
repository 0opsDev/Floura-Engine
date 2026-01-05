#ifndef SCRIPT_OBJECT_H
#define SCRIPT_OBJECT_H
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#pragma comment(lib, "lua54.lib")

class ScriptObject
{
public:
	sol::state luaState;

	float TimeAccumulator;

	float tickrate = 30;

	void LoadLua(sol::state& LuaState, std::string Path);

	void UpdateDelta();

	void runFunction(const std::string& name);

	void luaFunctions(); // object ID or Index
};

#endif // SCRIPT_OBJECT_H