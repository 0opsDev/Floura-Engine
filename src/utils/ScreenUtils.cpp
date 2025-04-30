#include "screenutils.h" 
#include "utils/init.h"
#include <iostream>
#include <GLFW/glfw3.h>

int windowedPosX= 0 , windowedPosY = 0;

bool ScreenUtils::isFullscreen;
bool ScreenUtils::isResizing;

void ScreenUtils::toggleFullscreen(GLFWwindow* window, GLFWmonitor* monitor, int windowedWidth, int windowedHeight) // needs to be fixed
{
	isFullscreen = !isFullscreen;
	if (isFullscreen) {

		// Save windowed mode dimensions and position
		glfwGetWindowPos(window, &windowedPosX, &windowedPosY);
		glfwGetWindowSize(window, &windowedWidth, &windowedHeight);

		const GLFWvidmode* mode = glfwGetVideoMode(monitor); // Get the video mode of the monitor

		glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate); // Switch to fullscreen
		isFullscreen = false;
	}
	else { 
		glfwSetWindowMonitor(window, NULL, windowedPosX, windowedPosY, windowedWidth, windowedHeight, 0); 
		isFullscreen = true;
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

ImVec2 prevSize = ImVec2(0, 0); // Initialize previous size

void ScreenUtils::UpdateViewportResize(){
	ImVec2 currentSize = ImGui::GetWindowSize();

	if (currentSize.x != prevSize.x || currentSize.y != prevSize.y) {
		// Window size has changed
		isResizing = true; }
	else { isResizing = false; }

	prevSize = currentSize; // Update the previous size
	
}