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
	static bool showViewportIcons;

	static bool imGuiPanels[]; // ImGui Panels
	static std::string FileTabs; // ImGui FileTabs
	static bool enableFB; // Change this as needed
	static bool enableDEF;
	static bool isWireframe;

	static std::string SelectedObjectType;
	static int SelectedObjectIndex;

	static float gPassTime;
	static float lPassTime;
	static float Render;
	static float physicsTime;

	static Texture FolderIcon; // Icon for folder in ImGui
	static Texture SaveIcon; // Icon for Save in ImGui
	static Texture ModelIcon; // Icon for Model in ImGui
	static Texture BillBoardIcon; // Icon for BillBoard in ImGui
	static Texture colliderIcon; // Icon for Collider in ImGui 
	static Texture pointLightIcon; // Icon for pointLightIcon in ImGui
	static Texture spotLightIcon; // Icon for spotLightIcon in ImGui 
	static Texture arrowIcon; // Icon for arrow in ImGui 
	static Texture crossIcon; // Icon for cross in ImGui 
	static Texture plusIcon; // Icon for plus in ImGui

	static void init();

	static void SystemInfomation();
	static void RenderWindow(GLFWwindow*& window, int windowedWidth, int windowedHeight);
	static void create();
	static void ShaderWindow();
	static void CameraWindow();
	static void LightWindow();
	static void PanelsWindow();

	static void PreformanceProfiler();

	static void TextEditor();

	static void audio();

	static void viewport();

	static void HierarchyList();

	static void addWindow(std::string typeString, bool &isOpen);

};

#endif // SCREEN_UTILS_CLASS_H