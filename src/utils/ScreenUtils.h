#ifndef SCREEN_UTILS_CLASS_H
#define SCREEN_UTILS_CLASS_H

#include "imgui/imgui_impl_glfw.h"

class ScreenUtils
{
public:

	static bool isFullscreen;

	static bool isResizing;

	static void toggleFullscreen(GLFWwindow* &window, int windowedWidth, int windowedHeight);

	static void UpdateViewportResize();

	static void UpdateWindowResize(GLFWwindow* window);


};

#endif // SCREEN_UTILS_CLASS_H