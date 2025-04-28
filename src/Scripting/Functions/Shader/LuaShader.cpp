#include "LuaShader.h"
#include "core/Main.h"

void luaShader::SetShader(sol::state& luaState) {
	luaState["SetShader"] = [](std::string ShaderType, int value) {
		if (ShaderType == "frag") {
			Main::FragNum = value;
		}
		else if (ShaderType == "vert") {
			Main::VertNum = value;
		}
		else if (ShaderType == "framebuffer") {
			std::cout << "[Lua] framebuffer not fully implemented yet" << std::endl;
		}
		else {
			std::cout << "[Lua] invalid shader type: " << std::endl;
		}

		Main::TempButton = -1;
		};
}