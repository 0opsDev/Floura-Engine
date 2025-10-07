#include "Main.h"
#include "utils/Init.h"
#include "utils/timeUtil.h" 
#include <thread>
#include <chrono>
#include "utils/timeAccumulator.h"
#include "utils/InputUtil.h"
#include "Scripting/ScriptRunner.h"
#include "File/File.h"
#include <UI/ImGui/ImGuiWindow.h>
#include <Sound/SoundRunner.h>
#include "Render.h"
#include "scene/tempscene.h"
#include "scene/scene.h"
#include <Gameplay/Player.h>
#include <Render/window/WindowHandler.h>
#include <Scene/LightingHandler.h>
#include "UI/OpenSceneWindow.h"
#include <windows.h>

bool Main::sleepState = true;
float Main::cameraSettings[3] = { 60.0f, 0.1f, 1000.0f }; // FOV, near, far // move this to camera class or something
TimeAccumulator TA2;

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
	glfwWindowHint(GLFW_DEPTH_BITS, 16); // DepthBuffer Bit
}

//Main Function
int main() // global variables do not work with threads
{
	CloseConsoleWindow();

	Main::sleepState = true;
	auto startInitTime = std::chrono::high_resolution_clock::now();
	init::initLog();// init logs (should always be before priniting anything)
	initGLFW(); // initialize glfw
	Main::loadSettings();

	ScriptRunner::init(Scene::sceneName + "/LuaStartup.json");
	SoundRunner::init();
	windowHandler::InitMainWidnow();

	//OpenSceneWindow::init();

	gladLoadGL(); // load open gl config

	//OpenSceneWindow::run(); // Run the OpenSceneWindow

	RenderClass::init(windowHandler::width, windowHandler::height);

	// INITIALIZE CAMERA
	Camera::InitCamera(windowHandler::width, windowHandler::height, Scene::initalCameraPos); 	// camera ratio pos

	//scene
	Scene::init();
	Scene::LoadScene(Scene::sceneName);
	Skybox::init(Skybox::DefaultSkyboxPath);

	//just a class to test stuff
	TempScene::init();

	Player::init();

	//Player::feetpos = glm::vec3(Camera::Position.x, (Camera::Position.y - Camera::PlayerHeightCurrent), Camera::Position.z);
	auto stopInitTime = std::chrono::high_resolution_clock::now();
	auto initDuration = std::chrono::duration_cast<std::chrono::microseconds>(stopInitTime - startInitTime);
	if (init::LogALL || init::LogSystems) std::cout << "init Duration: " << initDuration.count() / 1000000.0 << std::endl;

	while (!glfwWindowShouldClose(windowHandler::window)) // GAME LOOP
	{
		RenderClass::ClearFramebuffers(); // Clear Framebuffers
		TimeUtil::updateDeltaTime(); // Update delta time
		ScriptRunner::update();
		InputUtil::UpdateCurrentKey();

		if (!Camera::s_DoGravity) { Camera::DoJump = true; };

		auto startInitTime2 = std::chrono::high_resolution_clock::now();

		Scene::Update(); // keep scene above player

		TempScene::Update(); // Update scene
		Camera::Inputs(windowHandler::window);
		Player::update();

		RenderClass::Render(windowHandler::window, windowHandler::width, windowHandler::height);
		if (FEImGuiWindow::imGuiEnabled) { 

			TA2.update();
			if (TA2.Counter >= 1 / 10.0f) {
				if (glfwGetKey(windowHandler::window, GLFW_KEY_F1) == GLFW_PRESS) { FEImGuiWindow::imGuiPanels[0] = !FEImGuiWindow::imGuiPanels[0]; }
				//std::cout << "update" << std::endl;
				TA2.reset();
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
	TempScene::Delete(); // Delete scene
	SoundRunner::Delete();
	Scene::Delete();
	glfwDestroyWindow(windowHandler::window), glfwTerminate(); // Kill opengl
	return 0;
}



void Main::loadSettings() {
	// Load Settings.json
	std::ifstream settingsFile("Settings/Settings.json");
	if (settingsFile.is_open()) {
		json settingsData;
		settingsFile >> settingsData;
		settingsFile.close();


		if (settingsData[0].contains("Resolution")) {

			//Resolution
			// width
			windowHandler::width = settingsData[0]["Resolution"][0];
			Framebuffer::tempWidth = settingsData[0]["Resolution"][0];

			// height
			windowHandler::height = settingsData[0]["Resolution"][1];
			Framebuffer::tempHeight = settingsData[0]["Resolution"][1];

		}
		if (settingsData[0].contains("Sensitivity")) {
			Camera::sensitivity = glm::vec2(settingsData[0]["Sensitivity"][0].get<float>(), settingsData[0]["Sensitivity"][1].get<float>());
		}
		else {
			Camera::sensitivity = glm::vec2(100.0f, 100.0f);
		}

		windowHandler::doVsync = settingsData[0]["Vsync"];
		//Main::cameraSettings[0] = settingsData[0]["FOV"];
		Scene::sceneName = "Scenes/" + settingsData[0]["Scene"].get<std::string>();

		FEImGuiWindow::imGuiEnabled = settingsData[0]["imGui"];

		if (init::LogALL || init::LogSystems) std::cout << "Loaded settings from Settings.json" << std::endl;

	}
	else {
		std::cerr << "Failed to open Settings/Settings.json" << std::endl;
	}
}

void Main::saveSettings() {
	try {
		// Load the settings file
		std::ifstream settingsFile("Settings/Settings.json", std::ios::in);
		if (!settingsFile.is_open()) {
			if (init::LogALL || init::LogSystems) std::cout << "Failed to open Settings/Settings.json" << std::endl;
		}

		json settingsData;
		settingsFile >> settingsData;
		settingsFile.close();

		settingsData[0]["Vsync"] = windowHandler::doVsync;
		//settingsData[0]["FOV"] = Main::cameraSettings[0];

		settingsData[0]["Sensitivity"][0] = Camera::sensitivity.x;
		settingsData[0]["Sensitivity"][1] = Camera::sensitivity.y;

		// Write back to file
		std::ofstream outFile("Settings/Settings.json", std::ios::out);
		if (!outFile.is_open()) {
			if (init::LogALL || init::LogSystems) std::cout << ("Failed to write to Settings.json") << std::endl;
		}

		outFile << settingsData.dump(4);
		outFile.close();

		if (init::LogALL || init::LogSystems) std::cout << "Successfully updated Settings.json" << std::endl;

	}
	catch (const std::exception& e) {
		if (init::LogALL || init::LogSystems) std::cout << "Exception: " << e.what() << std::endl;
	}
}