#include "LuaScreen.h"
#include "Systems/utils/screenutils.h" 
#include "Systems/utils/SettingsUtil.h"

void luaScreen::SetScreen(sol::state& luaState) {
	luaState["SetWindow"] = [](std::string windowVariableType, std::string value) {

		if (windowVariableType == "Title") SettingsUtils::s_WindowTitle = value; // Title
		else if (windowVariableType == "Vsync" || "vsync") {
			//std::cout << "pass thru" << std::endl;
			if (value == "true") { ScreenUtils::setVSync(true); } // set vsync true
			if (value == "false") { ScreenUtils::setVSync(false); } // set vsync false
			if (value != "true" && value != "false") { if (init::LogALL || init::LogLua) std::cout << "[CPP] Exemption Lua SetWindow: 'Vysnc' can only be 'true' or 'false' But value is: " << value << std::endl; } //debug
		}
		else if (init::LogALL || init::LogLua) std::cout << "[CPP] Exemption Lua SetWindow: Unsupported 'windowVariableType' known as: " << windowVariableType << std::endl; // debug
		};
}