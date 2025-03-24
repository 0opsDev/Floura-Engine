#include "Init.h"

init::init() {

}

void init::initGLFW() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3), glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // Window Minimum and Maximum version
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //OpenGl Profile
	glfwWindowHint(GLFW_RESIZABLE, 0); // Start Resizable
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

void init::initLogo(GLFWwindow* window) {
	// change window icon
	// Icon Creation
	int iconW, iconH; // Width and Depth
	int iconChannels; // Image number (1)
	stbi_set_flip_vertically_on_load(false); // Disable Image Flipping On Load
	unsigned char* pixelsIcon = stbi_load("assets/Icons/Icon60B.tga", &iconW, &iconH, &iconChannels, STBI_rgb_alpha); // create var with imnage inside - tga because fast (hardly any compression)

	GLFWimage Iconinages[1]; // Create New "GLFWimage" VAR with "Iconinages at Channel (1)"
	Iconinages[0].width = iconW, Iconinages[0].height = iconH, Iconinages[0].pixels = pixelsIcon; // Write Aspect Ratio and Fragnment to photo 

	glfwSetWindowIcon(window, 1, Iconinages); // set the glfw window icon ("window", "Channel", "Image")
}