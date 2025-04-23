#include "Systems/utils/ScriptEngine.h"
#include "Systems/utils/init.h"
#include "Systems/utils/timeUtil.h"
#include <unordered_set>
#include <vector>
#include <string>
#include "core/Main.h"

float timeAccumulatorSE = 0;
std::vector<float> Modelx2, Modely2, Modelz2, RotW2, RotX2, RotY2, RotZ2, ScaleX2, ScaleY2, ScaleZ2;
std::vector<std::string> modelName2, Path2;
std::vector<bool>isCulling2	;
std::unordered_set<std::string> existingNames; // Track model names to avoid duplicates

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

// could we have a model.Clear function that clears the model
	/*
	// Clear arrays at the start of update
	Modelx2.clear();
	Modely2.clear();
	Modelz2.clear();
	RotW2.clear();
	RotX2.clear();
	RotY2.clear();
	RotZ2.clear();
	ScaleX2.clear();
	ScaleY2.clear();
	ScaleZ2.clear();
	modelName2.clear();
	Path2.clear();
	isCulling2.clear();
	existingNames.clear();
	*/
	luaState["ModelDraw"] = [](std::string Path, std::string modelName, bool isCulling, float Modelx, float Modely, float Modelz, float RotW, float RotX, float RotY, float RotZ, float ScaleX, float ScaleY, float ScaleZ) {
		//std::cout << "VALID" << std::endl;
		if (existingNames.find(modelName) == existingNames.end()) { // Only add if it's not a duplicate
			Modelx2.push_back(Modelx);
			Modely2.push_back(Modely);
			Modelz2.push_back(Modelz);
			RotW2.push_back(RotW);
			RotX2.push_back(RotX);
			RotY2.push_back(RotY);
			RotZ2.push_back(RotZ);
			ScaleX2.push_back(ScaleX);
			ScaleY2.push_back(ScaleY);
			ScaleZ2.push_back(ScaleZ);
			modelName2.push_back(modelName);
			Path2.push_back(Path);
			isCulling2.push_back(isCulling);
			existingNames.insert(modelName); // Mark the modelName as added
		}
		Main::updateModelLua(Path2, modelName2, isCulling2, Modelx2, Modely2, Modelz2, RotW2, RotX2, RotY2, RotZ2, ScaleX2, ScaleY2, ScaleZ2);
		return 0;
		}; // needs to be made so it doesnt clear when re ran



	luaState["update"](); // the issue is that for some reason two diff scripts cant run the same function at the same time 
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
