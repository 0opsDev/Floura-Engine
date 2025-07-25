#ifndef IMGUI_CAMERA_CLASS_H
#define IMGUI_CAMERA_CLASS_H


#include "Render/Shader/shaderClass.h"
#include <string>
#include "Camera/Camera.h"
#include <Render/Shader/Framebuffer.h>
#include <Core/Render.h>
#include "ImGuiWindow.h"

#include "ImGuiInclude.h"

//#include <Core/scene.h>


class FEImGuiWindow
{
public:
	static bool imGuiEnabled;

	static bool showViewportIcons;

	static bool imGuiPanels[]; // ImGui Panels
	static bool enableFB; // Change this as needed
	static bool enableDEF;
	static bool isWireframe;

	static std::string SelectedObjectType;
	static int SelectedObjectIndex;

	static float lPassTime;
	static float Render;

	static std::vector<std::string> ContentObjects;
	static std::vector<std::string> ContentObjectNames;
	static std::vector<std::string> ContentObjectPaths;
	static std::vector<std::string> ContentObjectTypes;

	static std::vector<std::string> MaterialObjectPaths;
	static std::vector<const char*> MaterialObjecNames;
	static int MaterialSelectedIndex;

	static Texture logoIcon; // Icon for logo in ImGui
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
	static Texture cameraIcon; // Icon for camera in ImGui
	static Texture skyboxIcon; // Icon for skybox in ImGui
	static Texture directLight; // Icon for directLight in ImGui
	static Texture materialIcon; // Icon for material in ImGui

	static void init();

	static void loadContentObjects(std::string path);
	static void saveContentObjects(std::string path);

	static void SystemInfomation();
	static void RenderWindow(GLFWwindow*& window, int windowedWidth, int windowedHeight);
	static void create();
	static void ShaderWindow();
	static void CameraWindow();
	static void PanelsWindow();

	static void PreformanceProfiler();

	static void TextEditor();

	static void viewport();

	static void HierarchyList();

	static void addWindow(std::string typeString, bool &isOpen);

	static void MaterialIndexUpdate();

	static void ModelWindow();

	static void BillBoardWindow();

	static void ColliderWindow();

	static void LightWindow();

	static void SkyBoxWindow();

	static void InspectorWindow();



	private:


};

#endif // SCREEN_UTILS_CLASS_H