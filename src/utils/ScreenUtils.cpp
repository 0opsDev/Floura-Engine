#include "screenutils.h" 
#include <iostream>
#include <GLFW/glfw3.h>
#include <Render/window/WindowHandler.h>
#include <glm/ext/vector_float3.hpp>
#include <glm/gtx/norm.hpp>
int windowedPosX= 0 , windowedPosY = 0;

bool ScreenUtils::isFullscreen;

// all the jank window code will stay here for the time being

void ScreenUtils::toggleFullscreen(GLFWwindow* &window, int windowedWidth, int windowedHeight) // needs to be fixed
{
	isFullscreen = !isFullscreen;
	if (isFullscreen) {

		// Save windowed mode dimensions and position
		glfwGetWindowPos(window, &windowedPosX, &windowedPosY);
		glfwGetWindowSize(window, &windowedWidth, &windowedHeight);

		const GLFWvidmode* mode = glfwGetVideoMode(windowHandler::primaryMonitor); // Get the video mode of the monitor

		glfwSetWindowMonitor(window, windowHandler::primaryMonitor, 0, 0, mode->width, mode->height, mode->refreshRate); // Switch to fullscreen
		isFullscreen = false;
	}
	else { 
		glfwSetWindowMonitor(window, NULL, windowedPosX, windowedPosY, windowedWidth, windowedHeight, 0); 
		isFullscreen = true;
		std::cout << "Restoring window to: " << windowedWidth << "x" << windowedHeight << " at position: " << windowedPosX << "," << windowedPosY << std::endl;
	} // Switch to windowed mode
}