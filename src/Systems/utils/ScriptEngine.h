#ifndef SCRIPT_ENGINE_H
#define SCRIPT_ENGINE_H

#include <Systems/utils/include.h>
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#pragma comment(lib, "lua54.lib")

class ScriptEngine
{
public:

	ScriptEngine(std::string Name, std::string Path); //Constructor

	void LoadLua(sol::state& LuaState, std::string Path);

	void init();

	void update();

	void UpdateDelta();

	void runFunction(const std::string& name);
};

#endif // SCRIPT_ENGINE_H