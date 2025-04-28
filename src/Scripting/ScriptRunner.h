#ifndef SCRIPT_RUNNER_H
#define SCRIPT_RUNNER_H

#include <Scripting/ScriptEngine.h>
#include <json/json.hpp>

using json = nlohmann::json;

class ScriptRunner
{
public:

	static void init();

	static void update();

};

#endif // SCRIPT_ENGINE_H