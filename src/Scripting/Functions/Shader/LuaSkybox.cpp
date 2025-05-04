#include "LuaSkybox.h"
#include <Render/Shader/SkyBox.h>

void luaSkybox::SetSkybox(sol::state& luaState) {
	luaState["SetSkybox"] = [](std::string SkyBoxPath) {
		Skybox::LoadSkyBoxTexture(SkyBoxPath);
	};
}