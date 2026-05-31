#ifndef WINDOW_HANDLER_CLASS_H
#define WINDOW_HANDLER_CLASS_H
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <glm/vec2.hpp>

class windowHandler
{
public:

	static GLFWmonitor* primaryMonitor;
	const static GLFWvidmode* videoMode;
	static GLFWwindow* window;
	static unsigned int width, height;
	static float window_width, window_height;
	static std::string s_WindowTitle; //"OpenGL Window"
	static bool doVsync;
	static double mouseX, mouseY;

	static GLFWmonitor* fetchPrimaryWindow();

	const static GLFWvidmode* fetchVideoMode(GLFWmonitor* Monitor);

	static GLFWwindow* createWindow(int width, int height, const char* windowTitle, GLFWmonitor* Monitor, GLFWwindow* share);

	static void SetScreenSize(GLFWwindow* window, unsigned int width, unsigned int height);

	static void SetWindowIcon(GLFWwindow* window, std::string path);

	static void setVSync(bool enabled);

	static void InitMainWindow();
	
	static void PollMousePositionsMainWindow();
	
	static void updateMouseLock(bool enabled, int w, int h);
	
	static void addWindowDropCallback(GLFWwindow* window);
	
	static void dropCallback(GLFWwindow* window, int count, const char** paths);
	
};
#endif