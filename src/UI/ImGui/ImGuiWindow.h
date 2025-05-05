#ifndef IMGUI_CAMERA_CLASS_H
#define IMGUI_CAMERA_CLASS_H

#include "imgui/imgui_impl_glfw.h"
#include "Render/Shader/shaderClass.h"
#include "camera/Camera.h"


class ImGuiCamera
{
public:

	static bool imGuiPanels[4]; // ImGui Panels
	static bool enableFB; // Change this as needed
	static bool enableLinearScaling;

	static float resolutionScale;
	static float prevResolutionScale; // Initialize previous scale
	static float prevEnableLinearScaling; // Initialize previous scale

	static char UniformInput[64]; // Zero-initialized buffer
	static float UniformFloat[3]; // Zero-initialized array
	static bool isWireframe;
	//static void imGuiMAIN(
	//	GLFWwindow* window, Shader shaderProgramT, GLFWmonitor* monitorT, Camera camera,
	//	unsigned int& frameBufferTexture, unsigned int& RBO, unsigned int& FBO, unsigned int& frameBufferTexture2
	//);

};

#endif // SCREEN_UTILS_CLASS_H