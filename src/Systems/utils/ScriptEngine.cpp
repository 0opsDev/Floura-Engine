#include "Systems/utils/ScriptEngine.h"
#include "Systems/utils/init.h"
#include "Systems/utils/timeUtil.h"

float timeAccumulatorSE = 0;
sol::state luaState; //create new lua state
// Constructor
ScriptEngine::ScriptEngine(std::string FunctionName, std::string Path) { //creates object
	LoadLua(luaState, Path); //load lua file
}

void ScriptEngine::LoadLua(sol::state& LuaState, std::string Path) {
	LuaState.open_libraries(sol::lib::base, sol::lib::io, sol::lib::math, sol::lib::table);
	// Step 1) Load & Parse File
	try
	{
		LuaState.safe_script_file(Path);
		if (init::LogALL || init::LogLua) std::cout << "[CPP LoadLua] Lua File read OK! at: " << Path << std::endl;
	}
	catch (const sol::error& e)
	{
		// Something went wrong with loading this script
		if (init::LogALL || init::LogLua) std::cout << "[CPP LoadLua] Lua File read ERROR! at: " << Path << std::endl;
		if (init::LogALL || init::LogLua) std::cout << std::string(e.what()) << "\n";
	}
}

//init
void ScriptEngine::init() {
	float x1 = luaState["init"]();
	if (init::LogALL || init::LogLua) std::cout << "[CPP runLuaInitFunc] init returned: " << x1 << std::endl;
}

void ScriptEngine::update() {
	luaState["update"]();
}

void ScriptEngine::UpdateDelta() {
	timeAccumulatorSE += TimeUtil::s_DeltaTime;
	if (timeAccumulatorSE >= 0.016f) {
		luaState["UpdateDelta"]();
		timeAccumulatorSE = 0;
	}
}