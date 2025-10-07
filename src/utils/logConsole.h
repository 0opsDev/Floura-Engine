#ifndef LOG_CONSOLE_CLASS_H	
#define LOG_CONSOLE_CLASS_H

#include <iostream>
#include <vector>
#include <string>

class LogConsole
{
public:

	static std::vector<std::string> logs;

	static void print(const std::string& message);
	//static void clear();


private:
	static bool doPrintToConsole;
};
#endif // LOG_CONSOLE_CLASS_H