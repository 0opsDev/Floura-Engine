#include "ScriptObject.h"
#include "Systems/util/UUID.h"
#include <utils/timeUtil.h>
#include <glm/glm.hpp>

ScriptObject::ScriptObject(std::string name)
{
	ScriptObject::UUID = UUID::returnHandle();
	ScriptObject::name = name;
}

void ScriptObject::loadScript(std::string path)
{
	ScriptObject::path = path;
	didInit = false;
	luaState.open_libraries(sol::lib::base, sol::lib::io, sol::lib::math, sol::lib::table);

	try
	{
		//std::cout << "loading: " << path << std::endl;
		luaState.safe_script_file(path);
	}
	catch (const sol::error& e)
	{
		//std::cout << "failed: " << path << std::endl;
		std::cout << std::string(e.what()) << "\n";
	}

	initEngineSpecificTables();
}

void ScriptObject::scriptInit()
{
	if (luaState["init"].valid()) {
		luaState["init"]();
		didInit = true;
	}
}

void ScriptObject::createTable(const char* tableName)
{
	luaState.create_named_table(tableName);
}

void ScriptObject::setUniform(const char* uniform, sol::table table, sol::object value)
{
	table[uniform] = value;
}

sol::object ScriptObject::getUniform(const char* uniform, sol::table table)
{
	return table[uniform];
}

void ScriptObject::initEngineSpecificTables()
{
	timeTable = getOrCreateTable("time");
}

void ScriptObject::updateEngineSpecficTables()
{
	setUniform("deltaTime", timeTable, sol::make_object(luaState, TimeUtil::deltatime));
	setUniform("lastFrameTime", timeTable, sol::make_object(luaState, TimeUtil::lastFrameTime));
	setUniform("frameRate", timeTable, sol::make_object(luaState, TimeUtil::frameRate));
	setUniform("time", timeTable, sol::make_object(luaState, TimeUtil::time));
}

void ScriptObject::scriptUpdate()
{
	updateEngineSpecficTables();

	if (!didInit)
	{
		ScriptObject::scriptInit();
	}

	if (luaState["update"].valid()) {
		luaState["update"]();
	}
}
