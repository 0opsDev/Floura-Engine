#include "Systems/utils/ScriptEngine.h"
#include "Systems/utils/init.h"
#include "Systems/utils/timeUtil.h"
#include <unordered_set>
#include <vector>
#include <string>
#include "core/Main.h"

float timeAccumulatorSE = 0;

std::vector<float> Modelx2, Modely2, Modelz2;
std::vector<std::string> modelName2, Path2;
std::unordered_set<std::string> existingNames; // Track model names to avoid duplicates


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
	// Clear arrays at the start of update
	Modelx2.clear();
	Modely2.clear();
	Modelz2.clear();
	modelName2.clear();
	Path2.clear();
	existingNames.clear();

	// Step 5) Call Lua Function, which calls C++ Function!
	luaState["ModelDraw"] = [](std::string Path, std::string modelName, float Modelx, float Modely, float Modelz) {
		if (existingNames.find(modelName) == existingNames.end()) { // Only add if it's not a duplicate
			Modelx2.push_back(Modelx);
			Modely2.push_back(Modely);
			Modelz2.push_back(Modelz);
			modelName2.push_back(modelName);
			Path2.push_back(Path);
			existingNames.insert(modelName); // Mark the modelName as added
		}
		return 0;
		};

	luaState["update"]();
	//now we need to send these arrays to main.cpp
	// Pass stored model data to updateModelLua

	//std::cout << Path2[1] << std::endl;

		Main::updateModelLua(Path2, modelName2, Modelx2, Modely2, Modelz2);
}


void ScriptEngine::UpdateDelta() {
	timeAccumulatorSE += TimeUtil::s_DeltaTime;
	if (timeAccumulatorSE >= 0.016f) {
		luaState["UpdateDelta"]();



		timeAccumulatorSE = 0;
	}
}

void ScriptEngine::runFunction(const std::string& name) {
    if (name == "update") {
        update();
    }
    else if (name == "UpdateDelta") {
        UpdateDelta();
    }
    else if (name == "init") {
        init();
    }
    else {
        if (luaState[name].valid()) {
            luaState[name](); // Call the Lua function
        } else {
            std::cerr << "Lua function '" << name << "' not found or invalid." << std::endl;
        }
    }
}
