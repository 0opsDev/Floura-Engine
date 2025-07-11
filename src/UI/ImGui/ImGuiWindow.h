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


class ImGuiWindow
{
public:

	static bool imGuiPanels[]; // ImGui Panels
	static bool DebugPanels[];
	static std::string FileTabs; // ImGui FileTabs
	static bool enableFB; // Change this as needed
	static bool enableDEF;

	static char UniformInput[64]; // Zero-initialized buffer
	static float UniformFloat[3]; // Zero-initialized array
	static bool isWireframe;

	static std::string SelectedObjectType;
	static int SelectedObjectIndex;

	static float gPassTime;
	static float lPassTime;
	static float Render;
	static float physicsTime;

	static void SystemInfomation();
	static void RenderWindow(GLFWwindow*& window, int windowedWidth, int windowedHeight);
	static void create();
	static void ShaderWindow();
	static void CameraWindow();
	static void LightWindow();
	static void PanelsWindow();
	static void PhysicsWindow();

	static void DebugWindow();

	static void PreformanceProfiler();

	static void TextEditor();

	static void audio();

	static void viewport();

	static void ModelH();

	static void BillBoardH();

	static void HierarchyList();

};

#endif // SCREEN_UTILS_CLASS_H