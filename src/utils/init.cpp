#include "Init.h"
#include <json/json.hpp>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

bool init::LogALL = true;

bool init::LogObject = true;

bool init::LogSystems = true;

bool init::LogLua = true;

void init::initLog() {
	std::ifstream LogCfgfile("Settings/LogConfig.json");
	if (LogCfgfile.is_open()) {
		json LogCfgData;
		LogCfgfile >> LogCfgData;
		LogCfgfile.close();

		LogALL = LogCfgData[0]["LogALL"];
		LogSystems = LogCfgData[0]["LogSystems"];
		LogObject = LogCfgData[0]["LogObject"];
		LogLua = LogCfgData[0]["LogLua"];
		if (LogALL || LogSystems) {
			std::cout << "LogALL: " << LogALL << std::endl;
			std::cout << "LogSystems: " << LogSystems << std::endl;
			std::cout << "LogObject: " << LogObject << std::endl;
			std::cout << "LogLua: " << LogLua << std::endl;
			std::cout << "Loaded settings from LogConfig.json" << std::endl;
		}
	}
	else {
		std::cout << "Failed to open Settings/LogConfig.json" << std::endl;
	}
}