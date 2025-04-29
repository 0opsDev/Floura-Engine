#ifndef SCRIPT_ENGINE_H
#define SCRIPT_ENGINE_H

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#pragma comment(lib, "lua54.lib")

class ScriptEngine
{
public:
	sol::state luaState;

	float TimeAccumulator;

	float ScriptEngine::tickrate = 30;

	ScriptEngine(std::string Name, std::string Path); //Constructor

	void LoadLua(sol::state& LuaState, std::string Path);

	void UpdateDelta();

	void runFunction(const std::string& name);

	void luaFunctions();
};

#endif // SCRIPT_ENGINE_H