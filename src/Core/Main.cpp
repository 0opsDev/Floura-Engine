#include "Main.h"
#include "utils/timeUtil.h" 
#include <thread>
#include <chrono>
#include "Scripting/ScriptRunner.h"
#include "File/File.h"
#include <UI/ImGui/ImGuiWindow.h>
#include "Render.h"
#include "scene/tempscene.h"
#include "scene/scene.h"
#include <Gameplay/Player.h>
#include <Render/window/WindowHandler.h>
#include <Scene/LightingHandler.h>
#include "UI/OpenSceneWindow.h"
#include <windows.h>
#include "Sound/SoundRunner.h"

bool Main::sleepState = true;
float Main::cameraSettings[3] = { 60.0f, 0.1f, 1000.0f }; // FOV, near, far // move this to camera class or something
float Counter;

void CloseConsoleWindow() {
	HWND hwnd = GetConsoleWindow();
	if (hwnd != nullptr) {
		FreeConsole();
		PostMessage(hwnd, WM_CLOSE, 0, 0);
	}
}

void initGLFW()
{
	// GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4), glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6); // Window Minimum and Maximum version
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //OpenGl Profile
	glfwWindowHint(GLFW_RESIZABLE, 1); // Start Resizable
	glfwWindowHint(GLFW_MAXIMIZED, 0); // Start Maximized
	glfwWindowHint(GLFW_DEPTH_BITS, 24); // DepthBuffer Bit
	glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);
	//glfwWindowHint(GLFW_SAMPLES, 4);
}

//Main Function
int main() // global variables do not work with threads
{
	//CloseConsoleWindow();

	Main::sleepState = true;
	auto startInitTime = std::chrono::high_resolution_clock::now();
	initGLFW(); // initialize glfw
	FileClass::loadSettings();

	ScriptRunner::init(Scene::sceneName + "/LuaStartup.json");
	SoundRunner::init();
	windowHandler::InitMainWidnow();

	//OpenSceneWindow::init();

	gladLoadGL(); // load open gl config

	//OpenSceneWindow::run(); // Run the OpenSceneWindow

	RenderClass::init(windowHandler::width, windowHandler::height);

	// INITIALIZE CAMERA
	Camera::InitCamera(windowHandler::width, windowHandler::height, Scene::initalCameraPos); 	// camera ratio pos
	Skybox::init(); 

	//scene
	Scene::init();
	Scene::LoadScene(Scene::sceneName); // could we thread this? // scene exsists already, we just need to add to the array 

	//two classes to test stuff
	TempScene::init();
	Player::init();

	//Player::feetpos = glm::vec3(Camera::Position.x, (Camera::Position.y - Camera::PlayerHeightCurrent), Camera::Position.z);
	auto stopInitTime = std::chrono::high_resolution_clock::now();
	auto initDuration = std::chrono::duration_cast<std::chrono::microseconds>(stopInitTime - startInitTime);
	std::cout << "init Duration: " << initDuration.count() / 1000000.0 << std::endl;

	while (!glfwWindowShouldClose(windowHandler::window)) // GAME LOOP
	{
		auto startInitTime2 = std::chrono::high_resolution_clock::now();

		TimeUtil::updateDeltaTime(); // Update delta time
		IdManager::update();
		ScriptRunner::update();
		Camera::Inputs(windowHandler::window);
		Scene::Update();
		RenderClass::ClearFramebuffers(); // Clear Framebuffers
		TempScene::Update();

		Player::update();

		RenderClass::Render(windowHandler::window, windowHandler::width, windowHandler::height);

		if (FEImGuiWindow::imGuiEnabled) { 
			Counter += TimeUtil::s_DeltaTime;
			if (Counter >= 1 / 10.0f) {
				if (glfwGetKey(windowHandler::window, GLFW_KEY_F1) == GLFW_PRESS) { FEImGuiWindow::imGuiPanels[0] = !FEImGuiWindow::imGuiPanels[0]; }
				Counter = 0;
			}
			if (FEImGuiWindow::imGuiPanels[0]) 
				FEImGuiWindow::Update();
		}

		RenderClass::Swapchain(windowHandler::window); // tip to self, work down to up (lines)

		auto stopInitTime2 = std::chrono::high_resolution_clock::now();
		auto initDuration2 = std::chrono::duration_cast<std::chrono::microseconds>(stopInitTime2 - startInitTime2);
		FEImGuiWindow::Render = (initDuration2.count() / 1000.0);

	}
	// Cleanup: Delete all objects on close
	Main::sleepState = false;

	if (FEImGuiWindow::imGuiEnabled)
	{
		ImGui_ImplOpenGL3_Shutdown(), ImGui_ImplGlfw_Shutdown(), ImGui::DestroyContext(); // Kill ImGui
	}
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