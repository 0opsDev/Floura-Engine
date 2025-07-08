#include "LuaSound.h"


void LuaSound::createSound(sol::state& luaState) {
	luaState["CreateSound"] = [this](std::string path) {
		Sound.CreateSound(path, "line 6 luasound please add name later"); // we can create a array of sounds, with name, iteration with the correct name will run the usesound at the same iteration
		};
}
void LuaSound::UseSound(sol::state& luaState) {

}