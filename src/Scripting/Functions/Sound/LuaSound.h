#ifndef LUA_SOUND_H
#define LUA_SOUND_H
#include "Sound/SoundProgram.h"

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#pragma comment(lib, "lua54.lib")

class LuaSound
{
public:
	SoundProgram Sound;

	void createSound(sol::state& luaState);
	void UseSound(sol::state& luaState);
};

#endif // LUA_SOUND_H