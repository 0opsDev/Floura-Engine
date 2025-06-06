#include "Scripting/ScriptEngine.h"
#include "utils/init.h"
#include "utils/timeUtil.h"
#include <unordered_set>
#include <vector>
#include <string>
#include "utils/SettingsUtil.h"
#include <json/json.hpp>
#include "Scripting/Functions/ScriptIncludes.h"
#include "utils/timeAccumulator.h"

// Constructor
ScriptEngine::ScriptEngine(std::string FunctionName, std::string Path) { //creates object
	auto startInitTime = std::chrono::high_resolution_clock::now();
	if (init::LogALL || init::LogLua) { std::cout << "[CPP] Lua script created: " << FunctionName << std::endl; }
	LoadLua(luaState, Path); //load lua file
	auto stopInitTime = std::chrono::high_resolution_clock::now();
	auto initDuration = std::chrono::duration_cast<std::chrono::microseconds>(stopInitTime - startInitTime);
	if (init::LogALL || init::LogLua) std::cout << "[CPP] Lua Interpret Duration: " << initDuration.count() / 1000000.0 << std::endl;
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

void ScriptEngine::UpdateDelta() {
	TimeAccumulator += TimeUtil::s_DeltaTime;
	if (TimeAccumulator >= 1/ tickrate) {

		luaFunctions();

		tickrate = luaState["UpdateDelta"]();
		//std::cout << tickrate << std::endl;
		TimeAccumulator = 0;
	}
}

void ScriptEngine::runFunction(const std::string& name) {
    if (name == "UpdateDelta") {
        UpdateDelta();
    }
    else {
        if (luaState[name].valid()) {
			luaFunctions();
            luaState[name](); // Call the Lua function
        } else {
            std::cerr << "Lua function '" << name << "' not found or invalid." << std::endl;
        }
    }
}

void ScriptEngine::luaFunctions(){
	luaModel LModel; LModel.DrawModel(luaState);
	luaScreen LScreen; LScreen.SetScreen(luaState);
	luaShader Lshader; Lshader.SetShader(luaState);
	LuaGetKey LGetKey; LGetKey.ProcessKeyPresses(luaState);
	luaSkybox Lskybox; Lskybox.SetSkybox(luaState);
	LuaCamera Lcamera; Lcamera.SetCameraPos(luaState); Lcamera.SetCameraSpeed(luaState);
}