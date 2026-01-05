#include "LuaCamera.h"
#include <camera/Camera.h>
#include "Scene/scene.h"

void LuaCamera::SetCameraPos(sol::state& luaState) {
	luaState["SetCameraPos"] = [](float x, float y, float z) {
		Scene::maincamera.Position.x = x;
		Scene::maincamera.Position.y = y;
		Scene::maincamera.Position.z = z;
	};
}

void LuaCamera::SetCameraSpeed(sol::state& luaState) {
	luaState["SetCameraSpeed"] = [](float Speed) {
		Scene::maincamera.s_scrollSpeed = Speed;
	};
}