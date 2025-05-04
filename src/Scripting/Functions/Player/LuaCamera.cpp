#include "LuaCamera.h"
#include <camera/Camera.h>

void LuaCamera::SetCameraPos(sol::state& luaState) {
	luaState["SetCameraPos"] = [](float x, float y, float z) {
		Camera::s_PositionMatrix.x = x;
		Camera::s_PositionMatrix.y = y;
		Camera::s_PositionMatrix.z = z;
	};
}