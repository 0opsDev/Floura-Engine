#include "screenutils.h" 

ScreenUtils::ScreenUtils() 
{
}

void ScreenUtils::toggleFullscreen(GLFWwindow* window, GLFWmonitor* monitor, bool isFullscreen, int windowedPosX, int windowedPosY, int windowedWidth, int windowedHeight)
{

	isFullscreen = !isFullscreen;
	if (isFullscreen) {

		// Save windowed mode dimensions and position
		glfwGetWindowPos(window, &windowedPosX, &windowedPosY);
		glfwGetWindowSize(window, &windowedWidth, &windowedHeight);

		const GLFWvidmode* mode = glfwGetVideoMode(monitor); // Get the video mode of the monitor

		glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate); // Switch to fullscreen
	}
	else { glfwSetWindowMonitor(window, NULL, windowedPosX, windowedPosY, windowedWidth, windowedHeight, 0); } // Switch to windowed mode
}

void ScreenUtils::setVSync(bool enabled) {
	glfwSwapInterval(enabled ? 1 : 0); //Toggles Vsync
}

void ScreenUtils::SetScreenSize(GLFWwindow* window, unsigned int width, unsigned int height) {
	glViewport(0, 0, width, height);
	glfwSetWindowSize(window, width, height);
}