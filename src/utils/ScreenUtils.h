#ifndef SCREEN_UTILS_CLASS_H
#define SCREEN_UTILS_CLASS_H

#include "imgui/imgui_impl_glfw.h"

class ScreenUtils
{
public:

	static bool isFullscreen;

	static bool isResizing;

	static void toggleFullscreen(GLFWwindow* window, GLFWmonitor* monitor, int windowedWidth, int windowedHeight);

	static void setVSync(bool enabled);

	static void SetScreenSize(GLFWwindow* window, unsigned int width, unsigned int height);

	static void UpdateViewportResize();


};

#endif // SCREEN_UTILS_CLASS_H