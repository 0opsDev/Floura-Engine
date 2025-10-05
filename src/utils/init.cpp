#include "Init.h"
#include <json/json.hpp>
#include <fstream>
#include <iostream>
#include <GLFW/glfw3.h>
#include<stb/stb_image.h>
#include "imgui/imgui_impl_opengl3.h"

using json = nlohmann::json;

bool init::LogALL = true;

bool init::LogModel = true;

bool init::LogSound = true;

bool init::LogSystems = true;

bool init::LogLua = true;

void init::initLog() {
	std::ifstream LogCfgfile("Settings/LogConfig.json");
	if (LogCfgfile.is_open()) {
		json LogCfgData;
		LogCfgfile >> LogCfgData;
		LogCfgfile.close();

		LogALL = LogCfgData[0]["LogALL"];
		LogSystems = LogCfgData[0]["LogSystems"];
		LogSound = LogCfgData[0]["LogSound"];
		LogModel = LogCfgData[0]["LogModel"];
		LogLua = LogCfgData[0]["LogLua"];
		if (LogALL || LogSystems) {
			std::cout << "LogALL: " << LogALL << std::endl;
			std::cout << "LogSystems: " << LogSystems << std::endl;
			std::cout << "LogModel: " << LogModel << std::endl;
			std::cout << "LogSound: " << LogSound << std::endl;
			std::cout << "LogLua: " << LogLua << std::endl;
			std::cout << "Loaded settings from LogConfig.json" << std::endl;
		}
	}
	else {
		std::cout << "Failed to open Settings/LogConfig.json" << std::endl;
	}
}

void init::initImGui(GLFWwindow* window) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460");

	ImGuiStyle& Style = ImGui::GetStyle();

	// General Colors
	Style.Colors[ImGuiCol_Text] = ImVec4(0.88f, 0.88f, 0.88f, 1.0f);
	Style.Colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.0f);
	Style.Colors[ImGuiCol_ChildBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.0f);
	Style.Colors[ImGuiCol_PopupBg] = ImVec4(0.12f, 0.08f, 0.15f, 1.0f);

	// Borders
	Style.Colors[ImGuiCol_Border] = ImVec4(0.19f, 0.19f, 0.25f, 1.0f);
	Style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	Style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.50f, 0.20f, 0.60f, 1.0f);
	Style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.70f, 0.30f, 0.80f, 1.0f);
	Style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.90f, 0.30f, 0.90f, 1.0f);


	// Frames
	Style.Colors[ImGuiCol_FrameBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.0f);
	Style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.70f, 0.30f, 0.80f, 1.0f);
	Style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.60f, 0.25f, 0.75f, 1.0f);

	// Buttons
	Style.Colors[ImGuiCol_Button] = ImVec4(0.50f, 0.20f, 0.60f, 1.0f);
	Style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.70f, 0.30f, 0.80f, 1.0f);
	Style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.40f, 0.15f, 0.55f, 1.0f);

	// Headers
	Style.Colors[ImGuiCol_Header] = ImVec4(0.50f, 0.20f, 0.60f, 1.0f);
	Style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.70f, 0.30f, 0.80f, 1.0f);
	Style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.40f, 0.15f, 0.55f, 1.0f);


	// Tabs
	Style.Colors[ImGuiCol_Tab] = ImVec4(0.19f, 0.10f, 0.25f, 1.0f);
	Style.Colors[ImGuiCol_TabActive] = ImVec4(0.50f, 0.20f, 0.60f, 1.0f);
	Style.Colors[ImGuiCol_TabHovered] = ImVec4(0.70f, 0.30f, 0.80f, 1.0f);
	Style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.25f, 0.15f, 0.35f, 1.0f);
	Style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.50f, 0.20f, 0.60f, 1.0f);

	// Title Bars
	Style.Colors[ImGuiCol_TitleBg] = ImVec4(0.40f, 0.15f, 0.55f, 1.0f);
	Style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.70f, 0.30f, 0.80f, 1.0f);
	Style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.25f, 0.10f, 0.30f, 1.0f);

	// Docking
	Style.Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.10f, 0.05f, 0.15f, 1.0f);
	Style.Colors[ImGuiCol_DockingPreview] = ImVec4(0.90f, 0.40f, 0.90f, 1.0f);

	// Resize Grip (Duplicated section from above, adjusted for consistency)
	Style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.50f, 0.20f, 0.60f, 1.0f);
	Style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.70f, 0.30f, 0.80f, 1.0f);
	Style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.90f, 0.30f, 0.90f, 1.0f);

	// Scrollbars
	Style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.0f);
	Style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.30f, 0.12f, 0.40f, 1.0f);
	Style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.50f, 0.20f, 0.60f, 1.0f);
	Style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.70f, 0.30f, 0.80f, 1.0f);

	// Sliders
	Style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.90f, 0.40f, 0.90f, 1.0f);
	Style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.60f, 0.20f, 0.70f, 1.0f);

	// Checkbox
	Style.Colors[ImGuiCol_CheckMark] = ImVec4(0.90f, 0.40f, 0.90f, 1.0f);

	// Menus
	Style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.20f, 0.10f, 0.25f, 1.0f);

	// Tooltips / Popups (Duplicated section, adjusted for consistency)
	Style.Colors[ImGuiCol_PopupBg] = ImVec4(0.10f, 0.05f, 0.15f, 1.0f);

	// Drag and drop
	Style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.90f, 0.40f, 0.90f, 0.9f);

	// Misc
	Style.Colors[ImGuiCol_Separator] = ImVec4(0.30f, 0.10f, 0.40f, 1.0f);
	Style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.70f, 0.30f, 0.80f, 1.0f);
	Style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.90f, 0.40f, 0.90f, 1.0f);

	// Rounding and Spacing (Unchanged, as requested)
	Style.FrameRounding = 8.0f;
	Style.WindowRounding = 6.0f;
	Style.ScrollbarRounding = 8.0f;
	Style.GrabRounding = 8.0f;

	Style.WindowPadding = ImVec2(8, 8);
	Style.FramePadding = ImVec2(6, 3);
	Style.ItemSpacing = ImVec2(7, 5);
}


void init::initGLenable(bool frontFaceSide) {
	// glenables
	// depth pass. render things in correct order. eg sky behind wall, dirt under water, not random order
	glEnable(GL_DEPTH_TEST); // Depth buffer
	glDepthFunc(GL_LESS);
	//glEnable(GL_STENCIL_TEST); //stencil buffer
	//glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glEnable(GL_CULL_FACE); // Culling
	glCullFace(GL_BACK);

	switch (frontFaceSide) { //currently set to false
	case true: { glFrontFace(GL_CW); break; } // inside facing
	case false: { glFrontFace(GL_CCW); break; } // outside facing
	}
}