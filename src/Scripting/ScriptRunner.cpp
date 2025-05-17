#include "Scripting/ScriptRunner.h"
#include "utils/init.h"
#include <fstream>

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

void ScriptRunner::init(std::string path) {
	auto startInitTime = std::chrono::high_resolution_clock::now();

	json config = loadScriptConfig(path);
	setupScripts(config);

	auto stopInitTime = std::chrono::high_resolution_clock::now();
	auto initDuration = std::chrono::duration_cast<std::chrono::microseconds>(stopInitTime - startInitTime);
	if (init::LogALL || init::LogLua) std::cout << "[CPP] Lua initialization Duration: " << initDuration.count() / 1000000.0 << std::endl;
}

void ScriptRunner::update() {
	runAllLoopFunctions();
}

void ScriptRunner::clearScripts() {
	for (auto& [name, info] : scripts) {
		delete info.engine; // Free allocated memory
	}
	scripts.clear(); // Remove all script entries
	if (init::LogALL || init::LogLua) std::cout << "[CPP] Lua Scripts Cleared" << std::endl;
}