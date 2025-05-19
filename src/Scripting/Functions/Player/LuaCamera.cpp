#include "LuaCamera.h"
#include <camera/Camera.h>

void LuaCamera::SetCameraPos(sol::state& luaState) {
	luaState["SetCameraPos"] = [](float x, float y, float z) {
		Camera::Position.x = x;
		Camera::Position.y = y;
		Camera::Position.z = z;
	};
}

void LuaCamera::SetCameraSpeed(sol::state& luaState) {
	luaState["SetCameraSpeed"] = [](float Speed) {
		Camera::s_scrollSpeed = Speed;
	};
}