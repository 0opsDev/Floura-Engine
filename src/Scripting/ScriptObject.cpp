#include "ScriptObject.h"
#include <utils/logConsole.h>
#include <utils/timeUtil.h>

void ScriptObject::LoadLua(sol::state& LuaState, std::string Path) {
	LuaState.open_libraries(sol::lib::base, sol::lib::io, sol::lib::math, sol::lib::table);
	// Step 1) Load & Parse File
	try
	{
		LuaState.safe_script_file(Path); // parse in scripts
		LogConsole::print("[CPP LoadLua] Lua File read OK! at: " + Path);
	}
	catch (const sol::error& e)
	{
		// Something went wrong with loading this script
		LogConsole::print("[CPP LoadLua] Lua File read ERROR! at: " + Path);
		LogConsole::print(std::string(e.what()));
	}
}

void ScriptObject::UpdateDelta() {
	TimeAccumulator += TimeUtil::s_DeltaTime;
	if (TimeAccumulator >= 1 / tickrate) {

		luaFunctions();

		tickrate = luaState["UpdateDelta"]();
		//std::cout << tickrate << std::endl;
		TimeAccumulator = 0;
	}
}

void ScriptObject::runFunction(const std::string& name) {
	if (name == "UpdateDelta") {
		UpdateDelta();
	}
	else {
		if (luaState[name].valid()) {
			luaFunctions();
			luaState[name](); // Call the Lua function
		}
		else {
			std::cerr << "Lua function '" << name << "' not found or invalid." << std::endl;
		}
	}
}

void ScriptObject::luaFunctions() {
}