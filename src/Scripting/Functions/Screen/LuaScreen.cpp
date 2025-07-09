#include "LuaScreen.h"
#include "utils/screenutils.h" 
#include "utils/SettingsUtil.h"
#include <Render/window/WindowHandler.h>

void luaScreen::SetScreen(sol::state& luaState) {
	luaState["SetWindow"] = [](std::string windowVariableType, std::string value) {

		if (windowVariableType == "Title") windowHandler::s_WindowTitle = value; // Title
		else if (windowVariableType == "Vsync" || "vsync") {
			//std::cout << "pass thru" << std::endl;
			if (value == "true") { windowHandler::setVSync(true); } // set vsync true
			if (value == "false") { windowHandler::setVSync(false); } // set vsync false
			if (value != "true" && value != "false") { if (init::LogALL || init::LogLua) std::cout << "[CPP] Exemption Lua SetWindow: 'Vysnc' can only be 'true' or 'false' But value is: " << value << std::endl; } //debug
		}
		else if (init::LogALL || init::LogLua) std::cout << "[CPP] Exemption Lua SetWindow: Unsupported 'windowVariableType' known as: " << windowVariableType << std::endl; // debug
		};
}