#ifndef SCRIPT_OBJECT_H
#define SCRIPT_OBJECT_H

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#include<string>
#include <xhash>
#pragma comment(lib, "lua54.lib")

class ScriptObject
{
public:
	uint64_t UUID;
	std::string name = "NULL";
	std::string path = "NULL";
	sol::state luaState;

	bool didInit = false;

	ScriptObject(std::string name);

	void loadScript(std::string path);

	void scriptUpdate();

	void scriptInit();

	void createTable(const char* tableName);

	sol::table getOrCreateTable(const char* tableName) {
		return luaState[tableName].get_or_create<sol::table>();
	}

	void setUniform(const char* uniform, sol::table table, sol::object value);

	sol::object getUniform(const char* uniform, sol::table table);

	private:
		sol::table timeTable;
		void initEngineSpecificTables();
		void updateEngineSpecficTables();
};

#endif // SCRIPT_OBJECT_H