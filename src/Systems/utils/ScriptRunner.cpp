#include "Systems/utils/ScriptRunner.h"

struct ScriptInfo {
	ScriptEngine* engine;
	std::vector<std::string> loopFunctions;
};

std::unordered_map<std::string, ScriptInfo> scripts;

json loadScriptConfig(const std::string& filepath) {
	std::ifstream file(filepath);
	if (!file.is_open()) {
		std::cerr << "Failed to open " << filepath << "\n";
		return {};
	}

	json config;
	file >> config;
	return config;
}

void setupScripts(const json& config) {
	for (const auto& scriptDef : config) {
		std::string name = scriptDef["name"];
		std::string path = "UserScripts/" + std::string(scriptDef["Path"]);

		if (scripts.find(name) != scripts.end())
			continue;

		ScriptEngine* engine = new ScriptEngine(name, path);

		// Run init functions
		if (scriptDef.contains("init")) {
			for (const auto& func : scriptDef["init"]) {
				engine->runFunction(func);
			}
		}

		ScriptInfo info{ engine };
		if (scriptDef.contains("loop")) {
			for (const auto& func : scriptDef["loop"]) {
				info.loopFunctions.push_back(func);
			}
		}

		scripts[name] = info;
	}
}

void runAllLoopFunctions() {
	for (auto& [name, info] : scripts) {
		for (const auto& func : info.loopFunctions) {
			info.engine->runFunction(func);
		}
	}
}

void ScriptRunner::init() {
	json config = loadScriptConfig("UserScripts/LuaStartup.json");
	setupScripts(config);
}

void ScriptRunner::update() {
	runAllLoopFunctions();
}