#ifndef IMGUI_CAMERA_CLASS_H
#define IMGUI_CAMERA_CLASS_H

#include "imgui/imgui_impl_glfw.h"
#include "Render/Shader/shaderClass.h"
#include <string>
#include "Camera/Camera.h"
#include <imgui/imgui_impl_opengl3.h>
#include <Render/Shader/Framebuffer.h>
#include <Core/Render.h>
//#include <Core/scene.h>


class ImGuiCamera
{
public:

	static bool imGuiPanels[]; // ImGui Panels
	static bool DebugPanels[];
	static std::string FileTabs; // ImGui FileTabs
	static bool enableFB; // Change this as needed

	static char UniformInput[64]; // Zero-initialized buffer
	static float UniformFloat[3]; // Zero-initialized array
	static bool isWireframe;

	static float gPassTime;
	static float lPassTime;
	static float Render;
	static float physicsTime;

	static void SystemInfomation();
	static void RenderWindow(GLFWwindow*& window, GLFWmonitor*& monitor, int windowedWidth, int windowedHeight);
	static void ShaderWindow();
	static void LightWindow();
	static void PanelsWindow();
	static void PhysicsWindow();

	static void DebugWindow();

	static void PreformanceProfiler();

	static void TextEditor();

	static void audio();

	static void viewport();

};

#endif // SCREEN_UTILS_CLASS_H