#include "Init.h"
#include <json/json.hpp>
#include <fstream>
#include <iostream>
#include <GLFW/glfw3.h>
#include<stb/stb_image.h>
#include "imgui/imgui_impl_opengl3.h"
#include <imgui_internal.h>
#include <glm/ext/vector_float3.hpp>
#include <glm/detail/func_exponential.inl> // not sure if this causes issues in dbg mode
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

glm::vec3 gammaCorrectI(glm::vec3 input)
{
	//return pow(input, glm::vec3(1.0f / 2.2));
	return input;
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
	//pow(0.35f, 1.0f / 2.2f), pow(input, 1.0f / 2.2f), pow(input, 1.0f / 2.2f), pow(input, 1.0f / 2.2f);

	glm::vec3 colourF = gammaCorrectI(glm::vec3(0.35f, 0.35f, 0.40f));
	glm::vec3 backgoundF = gammaCorrectI(glm::vec3(0.06f, 0.06f, 0.08f ));

	glm::vec3 colour_hoveredF = gammaCorrectI(glm::vec3( 0.55f, 0.55f, 0.60f ));
	glm::vec3 colour_activeF = gammaCorrectI(glm::vec3( 0.25f, 0.25f, 0.29f ));

	glm::vec3 bg_frameF = gammaCorrectI(glm::vec3( 0.08f, 0.08f, 0.11f));
	glm::vec3 bg_popupF = gammaCorrectI(glm::vec3( 0.09f, 0.09f, 0.12f ));
	glm::vec3 bg_borderF = gammaCorrectI(glm::vec3( 0.12f, 0.12f, 0.16f ));


	ImGuiStyle& Style = ImGui::GetStyle();
	ImVec4 colour = ImVec4(colourF.x, colourF.y, colourF.z, 1.0f);
	ImVec4 backgound = ImVec4(backgoundF.x, backgoundF.y, backgoundF.z, 1.0f);

	ImVec4 colour_hovered = ImVec4(colour_hoveredF.x, colour_hoveredF.y, colour_hoveredF.z, 1.0f);
	ImVec4 colour_active = ImVec4(colour_activeF.x, colour_activeF.y, colour_activeF.z, 1.0f);

	ImVec4 bg_frame = ImVec4(bg_frameF.x, bg_frameF.y, bg_frameF.z, 1.0f);
	ImVec4 bg_popup = ImVec4(bg_popupF.x, bg_popupF.y, bg_popupF.z, 1.0f);
	ImVec4 bg_border = ImVec4(bg_borderF.x, bg_borderF.y, bg_borderF.z, 1.0f);

	float dim_factor = 0.35f;
	ImVec4 colour_dimmed = ImLerp(backgound, colour, dim_factor);

	glm::vec3 colourG = gammaCorrectI(glm::vec3(0.88f, 0.88f, 0.88f));
	// General Colors
	Style.Colors[ImGuiCol_Text] = ImVec4(colourG.x, colourG.y, colourG.z, 1.0f);
	Style.Colors[ImGuiCol_WindowBg] = backgound;
	Style.Colors[ImGuiCol_ChildBg] = backgound;
	Style.Colors[ImGuiCol_PopupBg] = bg_popup;

	// Borders
	Style.Colors[ImGuiCol_Border] = bg_border;
	Style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

	// Frames
	Style.Colors[ImGuiCol_FrameBg] = bg_frame;
	Style.Colors[ImGuiCol_FrameBgHovered] = colour_hovered;
	Style.Colors[ImGuiCol_FrameBgActive] = colour_active;

	// Buttons
	Style.Colors[ImGuiCol_Button] = colour;
	Style.Colors[ImGuiCol_ButtonHovered] = colour_hovered;
	Style.Colors[ImGuiCol_ButtonActive] = colour_active;

	// Headers
	Style.Colors[ImGuiCol_Header] = colour;
	Style.Colors[ImGuiCol_HeaderHovered] = colour_hovered;
	Style.Colors[ImGuiCol_HeaderActive] = colour_active;

	// Tabs
	Style.Colors[ImGuiCol_Tab] = colour_dimmed;
	Style.Colors[ImGuiCol_TabActive] = colour;
	Style.Colors[ImGuiCol_TabHovered] = colour_hovered;
	Style.Colors[ImGuiCol_TabUnfocused] = colour_dimmed;
	Style.Colors[ImGuiCol_TabUnfocusedActive] = colour;

	// Title Bars
	Style.Colors[ImGuiCol_TitleBg] = colour_active;
	Style.Colors[ImGuiCol_TitleBgActive] = colour_hovered;
	Style.Colors[ImGuiCol_TitleBgCollapsed] = colour_dimmed;

	// Docking
	Style.Colors[ImGuiCol_DockingEmptyBg] = bg_frame;
	Style.Colors[ImGuiCol_DockingPreview] = colour_hovered;

	// Resize Grip
	Style.Colors[ImGuiCol_ResizeGrip] = colour;
	Style.Colors[ImGuiCol_ResizeGripHovered] = colour_hovered;
	Style.Colors[ImGuiCol_ResizeGripActive] = colour_hovered;

	// Scrollbars
	Style.Colors[ImGuiCol_ScrollbarBg] = bg_frame;
	Style.Colors[ImGuiCol_ScrollbarGrab] = colour_dimmed;
	Style.Colors[ImGuiCol_ScrollbarGrabHovered] = colour;
	Style.Colors[ImGuiCol_ScrollbarGrabActive] = colour_hovered;

	// Sliders
	Style.Colors[ImGuiCol_SliderGrab] = colour;
	Style.Colors[ImGuiCol_SliderGrabActive] = colour_active;

	// Checkbox
	Style.Colors[ImGuiCol_CheckMark] = colour_hovered;

	// Menus
	Style.Colors[ImGuiCol_MenuBarBg] = bg_border;

	// Drag and drop
	Style.Colors[ImGuiCol_DragDropTarget] = colour_hovered;

	// Misc Separators
	Style.Colors[ImGuiCol_Separator] = colour_dimmed;
	Style.Colors[ImGuiCol_SeparatorHovered] = colour_hovered;
	Style.Colors[ImGuiCol_SeparatorActive] = colour_hovered;

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