#include "LogConsole.h"

std::vector<std::string> LogConsole::logs;
bool LogConsole::doPrintToConsole = false;

void LogConsole::print(const std::string& message)
{
	if (message.empty()) return;


	/*
		if (logs.size() >= 1000) {
		logs.erase(logs.begin()); // Remove the oldest log entry if we exceed 1000 entries
	}
	*/

	if (doPrintToConsole) {
		std::cout << message << std::endl;
	}

	logs.push_back(message);
}