#include "Init.h"

init::init() {

}

void init::initGLFW() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3), glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // Window Minimum and Maximum version
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //OpenGl Profile
	glfwWindowHint(GLFW_RESIZABLE, 1); // Start Resizable
	glfwWindowHint(GLFW_MAXIMIZED, 1); // Start Maximized
	glfwWindowHint(GLFW_DEPTH_BITS, 16); // DepthBuffer Bit
}

void init::initImGui(GLFWwindow* window) {
	IMGUI_CHECKVERSION(), ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io, ImGui::StyleColorsDark();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	ImGui_ImplGlfw_InitForOpenGL(window, true), ImGui_ImplOpenGL3_Init("#version 330");

	//theme
	ImGuiStyle& Style = ImGui::GetStyle();
	Style.Colors[ImGuiCol_Text] = ImVec4(1, 1, 1, 1);
	Style.Colors[ImGuiCol_WindowBg] = ImVec4(0.157, 0.169, 0.188, 1);
	Style.Colors[ImGuiCol_CheckMark] = ImVec4(0, 1, 1, 1);
	Style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0, 0, 0, 1);
	Style.Colors[ImGuiCol_TitleBg] = ImVec4(0.118, 0.129, 0.141, 1);
	Style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.3, 0.3, 0.3, 1);
	Style.Colors[ImGuiCol_ButtonActive] = ImVec4(0, 1, 1, 1);
	Style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0, 1, 1, 1);
	Style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0, 1, 1, 1);
	Style.Colors[ImGuiCol_Border] = ImVec4(0, 0, 0, 1);
	Style.Colors[ImGuiCol_FrameBg] = ImVec4(0.212, 0.224, 0.243, 1);
	Style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.259, 0.271, 0.286, 1);
	Style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.447, 0.537, 0.855, 1);
	Style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.447, 0.537, 0.855, 1);
	Style.Colors[ImGuiCol_SliderGrab] = ImVec4(0, 1, 1, 1);
	Style.Colors[ImGuiCol_Button] = ImVec4(0.447, 0.537, 0.855, 1);
	Style.Colors[ImGuiCol_Tab] = ImVec4(0.447, 0.537, 0.855, 1);
}

void init::initBullet() {

}