#include "screenutils.h" 
#include "Systems/utils/init.h"
int windowedPosX= 0 , windowedPosY = 0;

void ScreenUtils::toggleFullscreen(GLFWwindow* window, GLFWmonitor* monitor, bool isFullscreen, int windowedWidth, int windowedHeight)
{
	isFullscreen = !isFullscreen;
	if (isFullscreen) {

		// Save windowed mode dimensions and position
		glfwGetWindowPos(window, &windowedPosX, &windowedPosY);
		glfwGetWindowSize(window, &windowedWidth, &windowedHeight);

		const GLFWvidmode* mode = glfwGetVideoMode(monitor); // Get the video mode of the monitor

		glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate); // Switch to fullscreen
	}
	else { 
		glfwSetWindowMonitor(window, NULL, windowedPosX, windowedPosY, windowedWidth, windowedHeight, 0); 
		std::cout << "Restoring window to: " << windowedWidth << "x" << windowedHeight << " at position: " << windowedPosX << "," << windowedPosY << std::endl;
	} // Switch to windowed mode
}

void ScreenUtils::setVSync(bool enabled) {
	glfwSwapInterval(enabled ? 1 : 0); //Toggles Vsync
}

void ScreenUtils::SetScreenSize(GLFWwindow* window, unsigned int width, unsigned int height) {
	glViewport(0, 0, width, height);
	glfwSetWindowSize(window, width, height);
	if (init::LogALL || init::LogSystems) std::cout << "set screensize: " << width << "*" << height << std::endl;
}