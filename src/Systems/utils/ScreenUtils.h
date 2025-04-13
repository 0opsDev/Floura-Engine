#ifndef SCREEN_UTILS_CLASS_H
#define SCREEN_UTILS_CLASS_H

#include <Systems/utils/include.h>

class ScreenUtils
{
public:

	static void toggleFullscreen(GLFWwindow* window, GLFWmonitor* monitor, bool isFullscreen, int windowedPosX, int windowedPosY, int windowedWidth, int windowedHeight);

	static void setVSync(bool enabled);

	static void SetScreenSize(GLFWwindow* window, unsigned int width, unsigned int height);

};

#endif // SCREEN_UTILS_CLASS_H