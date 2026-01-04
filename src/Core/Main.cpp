#include "Main.h"
#include "utils/timeUtil.h" 
#include <thread>
#include <chrono>
#include "Scripting/ScriptRunner.h"
#include "File/File.h"
#include <Editor/UI/ImGui/ImGuiWindow.h>
#include "Render.h"
#include "scene/tempscene.h"
#include "scene/scene.h"
#include <Gameplay/Player.h>
#include <Render/window/WindowHandler.h>
#include <Scene/LightingHandler.h>
#include "Editor/UI/ImGui/OpenSceneWindow.h"
#include <windows.h>
#include "Sound/SoundRunner.h"

bool Main::sleepState = true;
float Main::cameraSettings[3] = { 60.0f, 0.1f, 1000.0f }; // FOV, near, far // move this to camera class or something

void CloseConsoleWindow() { 
	HWND hwnd = GetConsoleWindow();
	if (hwnd != nullptr) { FreeConsole(); PostMessage(hwnd, WM_CLOSE, 0, 0); } 
}

int main()
{
	//CloseConsoleWindow();

	Main::sleepState = true;
	FileClass::loadSettings();
	ScriptRunner::init(Scene::sceneName + "/LuaStartup.json");
	SoundRunner::init();
	RenderClass::init(windowHandler::width, windowHandler::height);
	Scene::maincamera.InitCamera(windowHandler::width, windowHandler::height, Scene::initalCameraPos); 	// camera ratio pos
	Scene::init();
	Scene::LoadScene(Scene::sceneName);

	//two classes to test stuff
	TempScene::init();
	Player::init();

	while (!glfwWindowShouldClose(windowHandler::window)) // GAME LOOP
	{
		TimeUtil::update();
		ScriptRunner::update();
		Scene::maincamera.Inputs(windowHandler::window);
		Scene::maincamera.updateMatrix(Main::cameraSettings[0], Main::cameraSettings[1], Main::cameraSettings[2]); // Update: fov, near and far plane
		Scene::Update();
		RenderClass::ClearFramebuffers(); // Clear Framebuffers
		TempScene::Update();
		Player::update();
		RenderClass::Render(windowHandler::window, windowHandler::width, windowHandler::height);
	}
	// Cleanup: Delete all objects on close
	Main::sleepState = false;

	if (FEImGuiWindow::imGuiEnabled)
		ImGui_ImplOpenGL3_Shutdown(), ImGui_ImplGlfw_Shutdown(), ImGui::DestroyContext(); // Kill ImGui
	RenderClass::Cleanup();
	Skybox::Delete();
	Skybox::cleanup();
	TempScene::Delete(); // Delete scene
	SoundRunner::Delete();
	Scene::Delete();
	LightingHandler::cleanup();
	glfwDestroyWindow(windowHandler::window), glfwTerminate(); // Kill opengl
	return 0;
}