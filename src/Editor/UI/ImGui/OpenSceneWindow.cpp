#include "OpenSceneWindow.h"

/*
#include <Render/window/WindowHandler.h>
#include <utils/init.h>
#include <imgui_impl_opengl3.h>
#include <UI/ImGui/ImGuiWindow.h>

GLFWwindow* OpenSceneWindow::window;

void OpenSceneWindow::init()
{
	window = windowHandler::createWindow(800, 800, "Open Scene", NULL, NULL);
	init::initImGui(window); // Initialize ImGUI
	glfwMakeContextCurrent(window);	//make window current context

}

void OpenSceneWindow::run()
{
	while (!glfwWindowShouldClose(window)) // GAME LOOP
	{
		ImGui_ImplOpenGL3_NewFrame(); ImGui_ImplGlfw_NewFrame(); ImGui::NewFrame();
		//if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) { glfwSetWindowShouldClose(window, true); } // Escape to close window
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		OpenSceneWindow::menu();

		//finish render
		ImGui::Render(); // Renders the ImGUI elements
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
		glfwPollEvents(); // Poll events
	}
	ImGui_ImplOpenGL3_Shutdown(), ImGui_ImplGlfw_Shutdown(), ImGui::DestroyContext(); // Kill ImGui

	glfwDestroyWindow(window); // Kill opengl
	glfwMakeContextCurrent(windowHandler::window);	//make window current context
}

void OpenSceneWindow::menu()
{
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImGui::GetMainViewport()->Size);

	// Important flags for the main dockspace window
	ImGuiWindowFlags window_flags =
		ImGuiWindowFlags_NoDocking |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoNavFocus;

	ImGui::Begin("Open Scene");


	float button_height = 50.0f;
	float padding = 5.0f;
	ImVec2 window_size = ImGui::GetWindowSize();
	ImVec2 button_pos(ImGui::GetCursorScreenPos().x, window_size.y - button_height - padding);

	ImGui::SetCursorScreenPos(button_pos);

	ImGui::Dummy(ImVec2((ImGui::GetWindowWidth() / 2.0f) - 100.0f, 1 ));
	ImGui::SameLine();
	float buttonSize = 50.0f;

	if (ImGui::Button("Open", ImVec2(buttonSize, buttonSize))) {

	}
	ImGui::SameLine();
	if (ImGui::Button("New", ImVec2(buttonSize, buttonSize))) {

	}

	ImGui::End();
}
*/