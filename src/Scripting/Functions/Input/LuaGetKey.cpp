#include "LuaGetKey.h"
#include "Core/Main.h"
#include "Systems/utils/InputUtil.h"

void LuaGetKey::ProcessKeyPresses(sol::state& luaState) {
	luaState["FetchKey"](InputUtil::CurrentKey);
}
