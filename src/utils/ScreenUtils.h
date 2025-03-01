#ifndef SCREEN_UTILS_CLASS_H
#define SCREEN_UTILS_CLASS_H

#include <include.h>

class ScreenUtils
{
public:

	ScreenUtils();

	void toggleFullscreen(GLFWwindow* window, GLFWmonitor* monitor, bool isFullscreen, int windowedPosX, int windowedPosY, int windowedWidth, int windowedHeight);

};

#endif // SCREEN_UTILS_CLASS_H