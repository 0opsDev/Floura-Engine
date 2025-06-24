#include "LuaShader.h"
#include "core/Main.h"
#include <Core/Render.h>
#include <Core/File/File.h>

void luaShader::SetShader(sol::state& luaState) {
	luaState["SetShader"] = [](std::string ShaderType, int value) {
		if (value < 0) {
			value = 0;
			if (init::LogALL || init::LogLua) std::cout << "[Lua] SetShader value below zero, corrected to 0" << std::endl;
		}
		else if (ShaderType == "frag") {
			Main::FragNum = value;
		}
		else if (ShaderType == "vert") {
			Main::VertNum = value;
		}
		else if (ShaderType == "framebuffer") {
			if (init::LogALL || init::LogLua) std::cout << "[Lua] framebuffer not fully implemented yet" << std::endl;
		}
		else {
			if (init::LogALL || init::LogLua) std::cout << "[Lua] invalid shader type: " << std::endl;
		}

		FileClass::loadShaderProgram(Main::VertNum, Main::FragNum, RenderClass::shaderProgram);
		};
}