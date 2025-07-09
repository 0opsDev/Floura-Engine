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

void init::initGLFW() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4), glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6); // Window Minimum and Maximum version
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //OpenGl Profile
	glfwWindowHint(GLFW_RESIZABLE, 1); // Start Resizable
	glfwWindowHint(GLFW_MAXIMIZED, 0); // Start Maximized
	glfwWindowHint(GLFW_DEPTH_BITS, 16); // DepthBuffer Bit
}

void init::initImGui(GLFWwindow* window) {
	IMGUI_CHECKVERSION(), ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io, ImGui::StyleColorsDark();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	ImGui_ImplGlfw_InitForOpenGL(window, true), ImGui_ImplOpenGL3_Init("#version 460");

	ImGuiStyle& Style = ImGui::GetStyle();

	// General Colors
	Style.Colors[ImGuiCol_Text] = ImVec4(0.88f, 0.88f, 0.88f, 1.0f);  // Light gray text
	Style.Colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.0f);  // Dark background
	Style.Colors[ImGuiCol_Border] = ImVec4(0.19f, 0.19f, 0.19f, 1.0f);  // Subtle dark gray border

	// Buttons
	Style.Colors[ImGuiCol_Button] = ImVec4(0.26f, 0.29f, 0.48f, 1.0f);  // Discord blue
	Style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.34f, 0.38f, 0.68f, 1.0f);  // Lighter blue on hover
	Style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.22f, 0.38f, 1.0f);  // Darker blue when clicked

	// Frame Background
	Style.Colors[ImGuiCol_FrameBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.0f);  // Dark gray
	Style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.26f, 0.42f, 1.0f);  // Discord blue accent
	Style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.34f, 0.38f, 0.68f, 1.0f);  // Vibrant blue

	// Tabs
	Style.Colors[ImGuiCol_Tab] = ImVec4(0.19f, 0.19f, 0.19f, 1.0f);  // Soft dark gray tab
	Style.Colors[ImGuiCol_TabHovered] = ImVec4(0.34f, 0.38f, 0.68f, 1.0f);  // Vibrant blue on hover
	Style.Colors[ImGuiCol_TabActive] = ImVec4(0.26f, 0.29f, 0.48f, 1.0f);  // Discord blue

	// Checkmarks
	Style.Colors[ImGuiCol_CheckMark] = ImVec4(0.34f, 0.38f, 0.68f, 1.0f);  // Vibrant blue

	// Slider
	Style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.34f, 0.38f, 0.68f, 1.0f);  // Discord blue
	Style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.20f, 0.22f, 0.38f, 1.0f);  // Dark blue

	// Scrollbar
	Style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.0f);  // Dark gray background
	Style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.19f, 0.19f, 0.19f, 1.0f);  // Subtle dark gray
	Style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.26f, 0.29f, 0.48f, 1.0f);  // Discord blue
	Style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.34f, 0.38f, 0.68f, 1.0f);  // Vibrant blue

	// Curved Elements
	Style.FrameRounding = 8.0f;
	Style.WindowRounding = 6.0f;
	Style.ScrollbarRounding = 8.0f;

	// Padding
	Style.WindowPadding = ImVec2(8, 8);
	Style.FramePadding = ImVec2(6, 3);
	Style.ItemSpacing = ImVec2(7, 5);
}

void init::initGLenable(bool frontFaceSide ) {
    // glenables
    // depth pass. render things in correct order. eg sky behind wall, dirt under water, not random order
    glEnable(GL_DEPTH_TEST); // Depth buffer
    glDepthFunc(GL_LESS);
    glEnable(GL_STENCIL_TEST); //stencil buffer
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glEnable(GL_CULL_FACE); // Culling
    glCullFace(GL_BACK);

    switch (frontFaceSide) { //currently set to false
    case true: { glFrontFace(GL_CW); break; } // inside facing
    case false: { glFrontFace(GL_CCW); break; } // outside facing
    }
}