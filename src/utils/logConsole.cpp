#include "LogConsole.h"

std::vector<std::string> LogConsole::logs;
std::string LogConsole::priorPrint;

void LogConsole::antiDuplicatePrint(const std::string& message){
	if (message == priorPrint) return;
	std::cout << message << "\n";
	priorPrint = message;
}

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
		std::cout << message << "\n";
	}

	logs.push_back(message);
}