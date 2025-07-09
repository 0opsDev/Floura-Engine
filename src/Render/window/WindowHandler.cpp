#include "WindowHandler.h"
#include <utils/init.h>
#include <stb/stb_image.h>


GLFWmonitor* windowHandler::primaryMonitor;
const GLFWvidmode* windowHandler::videoMode;
GLFWwindow* windowHandler::window;
unsigned int windowHandler::width;
unsigned int windowHandler::height;
float windowHandler::window_width;
float windowHandler::window_height;
std::string windowHandler::s_WindowTitle = "OpenGL Window";\
bool windowHandler::doVsync = false;

GLFWmonitor* windowHandler::fetchPrimaryWindow() {
	GLFWmonitor* tempMonitor = glfwGetPrimaryMonitor();
	if (!tempMonitor) { std::cerr << "Failed to get primary monitor" << std::endl; glfwTerminate();}
	return tempMonitor;
}

const GLFWvidmode* windowHandler::fetchVideoMode(GLFWmonitor* Monitor) {
	const GLFWvidmode* tempvideoMode = glfwGetVideoMode(Monitor);
	if (!tempvideoMode) { std::cerr << "Failed to get video mode" << std::endl; glfwTerminate();}
	return tempvideoMode;
}

GLFWwindow* windowHandler::createWindow(int width, int height, const char* windowTitle, GLFWmonitor* Monitor, GLFWwindow* share) {
	GLFWwindow* window = glfwCreateWindow(width, height, windowTitle, Monitor, share); // create window
	// error checking
	if (window == NULL) std::cout << "failed to create window" << std::endl; //glfwTerminate();

	return window;
}

void windowHandler::SetScreenSize(GLFWwindow* window, unsigned int width, unsigned int height) {
	glViewport(0, 0, width, height);
	glfwSetWindowSize(window, width, height);
	if (init::LogALL || init::LogSystems) std::cout << "set screensize: " << width << "*" << height << std::endl;
}

void windowHandler::SetWindowIcon(GLFWwindow* window, std::string path) {
	// Icon Creation
	int iconW, iconH; // Width and Depth
	int iconChannels; // Image number (1)
	stbi_set_flip_vertically_on_load(false); // Disable Image Flipping On Load

	unsigned char* pixelsIcon = stbi_load((path).c_str(), &iconW, &iconH, &iconChannels, STBI_rgb_alpha);

	//change window icon
	GLFWimage Iconinages[1];
	//give glfw pixels, width and height
	Iconinages[0].width = iconW;
	Iconinages[0].height = iconH;
	Iconinages[0].pixels = pixelsIcon;

	//change to icon (what window, how many images, what image)
	glfwSetWindowIcon(window, 1, Iconinages);
}

void windowHandler::setVSync(bool enabled) {
	glfwSwapInterval(enabled ? 1 : 0); //Toggles Vsync
}


void windowHandler::InitMainWidnow() {
	// Get the primary monitor
	primaryMonitor = fetchPrimaryWindow();
	// Get the video mode of the primary monitor
	videoMode = fetchVideoMode(primaryMonitor);

	// fall back values if height & width is null
	if (width == 0) { width = videoMode->width; }
	if (height == 0) { height = videoMode->height; }
	if (window_width == 0) { window_width = videoMode->width; }
	if (window_height == 0) { window_height = videoMode->height; }

	// put primary window where null is for fullscreen
	window = createWindow(width, height, s_WindowTitle.c_str(), NULL, NULL);
	if (init::LogALL || init::LogSystems) std::cout << "Primary monitor resolution: " << width << "x" << height << std::endl;

	// window logo creation and assignment
	SetWindowIcon(window, "assets/Icons/Icon.png");
	glfwMakeContextCurrent(windowHandler::window);	//make window current context

}