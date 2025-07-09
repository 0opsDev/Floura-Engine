#include "Main.h"
#include"Render/Model/Model.h"
#include "Camera/Camera.h"
#include "utils/Init.h"
#include "utils/screenutils.h" 
#include <glm/gtx/string_cast.hpp>
#include "utils/timeUtil.h" 
#include <thread>
#include <chrono>
#include <utils/SettingsUtil.h>
#include "utils/timeAccumulator.h"
#include "utils/InputUtil.h"
#include "Render/Shader/Cubemap.h"
#include "Render/Shader/Framebuffer.h"
#include "Scripting/ScriptRunner.h"
#include "imgui/imgui_impl_opengl3.h"
#include "render/Shader/SkyBox.h"
#include "File/File.h"
#include <UI/ImGui/ImGuiWindow.h>
#include "Sound/SoundProgram.h"
#include <Sound/SoundRunner.h>
#include "Render.h"
#include "Render/Cube/CubeVisualizer.h"
#include <Physics/CubeCollider.h>
#include "Render/Cube/Billboard.h"
#include "tempscene.h"
#include "scene.h"
#include <Gameplay/Player.h>
#include <Render/window/WindowHandler.h>

int Main::VertNum = 0, Main::FragNum = 0;
bool Main::sleepState = true;

float Main::cameraSettings[3] = { 60.0f, 0.1f, 1000.0f }; // FOV, near, far

float PlayerHeight = 1.8f;
float CrouchHighDiff = 0.9f;

glm::vec3 Main::initalCameraPos = glm::vec3(0,0,0);

TimeAccumulator TA2;

//Main Function
int main() // global variables do not work with threads
{
	Main::sleepState = true;
	auto startInitTime = std::chrono::high_resolution_clock::now();
	init::initLog();// init logs (should always be before priniting anything)
	init::initGLFW(); // initialize glfw
	Main::LoadPlayerConfig();
	Main::loadSettings();
	//Main::loadEngineSettings();
	ScriptRunner::init(SettingsUtils::sceneName + "/LuaStartup.json");
	SoundRunner::init();

	windowHandler::InitMainWidnow();

	gladLoadGL(); // load open gl config

	Scene::LoadScene(SettingsUtils::sceneName);
	FileClass::loadShaderProgram(Main::VertNum, Main::FragNum, RenderClass::shaderProgram); // feed the shader prog real data << should take a shader file class
	RenderClass::shaderProgram.Activate(); // activate new shader program for use

	// INITIALIZE CAMERA
	Camera::InitCamera(windowHandler::width, windowHandler::height, Main::initalCameraPos); 	// camera ratio pos
	//std::cout << initalCameraPos.x << " " << initalCameraPos.y << " " << initalCameraPos.z << std::endl;
	//std::cout << Camera::Position.x << " " << Camera::Position.y << " " << Camera::Position.z << std::endl;
	RenderClass::init(windowHandler::window, windowHandler::width, windowHandler::height);
	// Model Loader

	TempScene::init(); // Initialize scene
	Player::init();

	//Player::feetpos = glm::vec3(Camera::Position.x, (Camera::Position.y - Camera::PlayerHeightCurrent), Camera::Position.z);
	auto stopInitTime = std::chrono::high_resolution_clock::now();
	auto initDuration = std::chrono::duration_cast<std::chrono::microseconds>(stopInitTime - startInitTime);
	if (init::LogALL || init::LogSystems) std::cout << "init Duration: " << initDuration.count() / 1000000.0 << std::endl;

	while (!glfwWindowShouldClose(windowHandler::window)) // GAME LOOP
	{
		//std::cout << RenderClass::CameraXYZ.x << " " << RenderClass::CameraXYZ.y << " " << RenderClass::CameraXYZ.z << std::endl;
		//std::cout << Camera::Position.x << " " << Camera::Position.y << " " << Camera::Position.z << std::endl;
		RenderClass::ClearFramebuffers(); // Clear Framebuffers

		TimeUtil::updateDeltaTime(); // Update delta time
		ScriptRunner::update();
		InputUtil::UpdateCurrentKey(windowHandler::window);

		if (glfwGetKey(windowHandler::window, GLFW_KEY_F2) == GLFW_PRESS) {
			Camera::s_DoGravity = false;
			CubeCollider::CollideWithCamera = false;
		}
		if (glfwGetKey(windowHandler::window, GLFW_KEY_F3) == GLFW_PRESS) {
			Camera::s_DoGravity = true;
			CubeCollider::CollideWithCamera = true;
		}
		//TA2
		TA2.update();
		if (TA2.Counter >= 1 / 10.0f) {
			if (glfwGetKey(windowHandler::window, GLFW_KEY_F1) == GLFW_PRESS) { ImGuiWindow::imGuiPanels[0] = !ImGuiWindow::imGuiPanels[0]; }
			//std::cout << "update" << std::endl;
			TA2.reset();
		}

		if (Camera::s_DoGravity) {
			//crouch 
			if (glfwGetKey(windowHandler::window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) { Camera::PlayerHeightCurrent = CrouchHighDiff; }
			else { Camera::PlayerHeightCurrent = PlayerHeight; }
		}

		if (!Camera::s_DoGravity) { Camera::DoJump = true; };

		auto startInitTime2 = std::chrono::high_resolution_clock::now();

		Scene::Update(); // keep scene above player

		TempScene::Update(); // Update scene
		Camera::Inputs(windowHandler::window);
		Player::update();

		RenderClass::Render(windowHandler::window, windowHandler::width, windowHandler::height);
		if (ImGuiWindow::imGuiPanels[0]) { Main::imGuiMAIN(windowHandler::window); }

		RenderClass::Swapchain(windowHandler::window); // tip to self, work down to up (lines)

		auto stopInitTime2 = std::chrono::high_resolution_clock::now();
		auto initDuration2 = std::chrono::duration_cast<std::chrono::microseconds>(stopInitTime2 - startInitTime2);
		ImGuiWindow::Render = (initDuration2.count() / 1000.0);

		//Player::isGrounded = false;
		Player::isColliding = false;
	}
	// Cleanup: Delete all objects on close
	Main::sleepState = false;

	ImGui_ImplOpenGL3_Shutdown(), ImGui_ImplGlfw_Shutdown(), ImGui::DestroyContext(); // Kill ImGui

	RenderClass::Cleanup();
	//FootSound.DeleteSound();
	Skybox::Delete();
	TempScene::Delete(); // Delete scene
	SoundRunner::Delete();
	Scene::Delete();
	//SolidColour.Delete();
	glfwDestroyWindow(windowHandler::window), glfwTerminate(); // Kill opengl
	return 0;
}

void Main::LoadPlayerConfig() {
	// Load PlayerConfig.json
	std::ifstream playerConfigFile("Settings/PlayerController.json");
	if (playerConfigFile.is_open()) {
		json playerConfigData;
		playerConfigFile >> playerConfigData;
		playerConfigFile.close();

		Camera::s_DoGravity = playerConfigData[0]["DoGravity"];
		if (init::LogALL || init::LogSystems) std::cout << "DoGravity: " << Camera::s_DoGravity << std::endl;

		PlayerHeight = playerConfigData[0]["PlayerHeight"];
		if (init::LogALL || init::LogSystems) std::cout << "PlayerHeight: " << PlayerHeight << std::endl;
		CrouchHighDiff = playerConfigData[0]["CrouchHighDiff"];
		if (init::LogALL || init::LogSystems) std::cout << "CrouchHighDiff: " << CrouchHighDiff << std::endl;

		if (init::LogALL || init::LogSystems) std::cout << "Loaded Player Config from Settings/PlayerConfig.json" << std::endl;
	}
	else {
		std::cerr << "Failed to open Settings/PlayerConfig.json" << std::endl;
	}
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
			SettingsUtils::tempWidth = settingsData[0]["Resolution"][0];

			// height
			windowHandler::height = settingsData[0]["Resolution"][1];
			SettingsUtils::tempHeight = settingsData[0]["Resolution"][1];

		}
		if (settingsData[0].contains("Sensitivity")) {
			Camera::sensitivity = glm::vec2(settingsData[0]["Sensitivity"][0].get<float>(), settingsData[0]["Sensitivity"][1].get<float>());
		}
		else {
			Camera::sensitivity = glm::vec2(100.0f, 100.0f);
		}


		windowHandler::doVsync = settingsData[0]["Vsync"];
		Main::cameraSettings[0] = settingsData[0]["FOV"];
		SettingsUtils::sceneName = "Scenes/" + settingsData[0]["Scene"].get<std::string>();

		ImGuiWindow::imGuiPanels[0] = settingsData[0]["imGui"];

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
		settingsData[0]["FOV"] = Main::cameraSettings[0];

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


// Holds ImGui Variables and Windows
void Main::imGuiMAIN(GLFWwindow* window) {
	//Tell Imgui a new frame is about to begin
	ImGui_ImplOpenGL3_NewFrame(); ImGui_ImplGlfw_NewFrame(); ImGui::NewFrame();

	ImGui::SetNextWindowPos(ImVec2(0, 0));
	// Create a full-screen docking space

	// Main docking window
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImGui::GetMainViewport()->Size);
	ImGui::Begin("Docking Window", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

	// Create a DockSpace
	ImGuiID dockspace_id = ImGui::GetID("MainDockspace");
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
	ImGui::End();
	//panel
	ImGuiWindow::PanelsWindow();
	// Rendering panel
	if (ImGuiWindow::imGuiPanels[1]) {

		ImGui::Begin("Rendering"); // ImGUI window creation

		//rendering here
		ImGuiWindow::RenderWindow(window, windowHandler::width, windowHandler::height);

		ImGuiWindow::ShaderWindow();
		// Lighting panel
		ImGuiWindow::LightWindow();

		ImGui::End();
	}
	// Camera panel
	if (ImGuiWindow::imGuiPanels[2]) {
		ImGuiWindow::CameraWindow();
		
	}

	if (ImGuiWindow::imGuiPanels[3]) {
		ImGuiWindow::viewport();
	}

	if (ImGuiWindow::imGuiPanels[5]) {
		ImGuiWindow::PhysicsWindow();
	}

	if (ImGuiWindow::imGuiPanels[6]) {
		ImGuiWindow::DebugWindow();
	}

	if (ImGuiWindow::imGuiPanels[7]) {
		ImGuiWindow::TextEditor();
	}

	if (ImGuiWindow::imGuiPanels[8]) {
		ImGuiWindow::audio();
	}

	if (ImGuiWindow::imGuiPanels[4]) {
		ImGui::Begin("Scene hierarchy"); // ImGUI window creation
		ImGui::Text("object hierarchy");

		ImGui::Spacing();
		if (ImGui::SmallButton("Save Current Scene")) {
			Scene::SaveScene(SettingsUtils::sceneName);
		}
		ImGui::Spacing();
		if (ImGui::SmallButton("Load Last Scene")) {
			Scene::LoadScene(SettingsUtils::sceneName);
		}
		ImGui::Spacing();
		if (ImGui::SmallButton("Unload Current Scene")) {
			Scene::Delete();
		}
		ImGui::Spacing();

		ImGuiWindow::create();

		ImGui::Spacing();

		ImGuiWindow::ModelH();

		ImGui::Spacing();

		ImGuiWindow::BillBoardH();

		ImGui::Spacing();
		if (ImGui::TreeNode("Sound")) {
			for (size_t i = 0; i < Scene::SoundObjects.size(); i++) {
				if (ImGui::TreeNode((Scene::SoundObjects[i].name).c_str())) {
					//ImGui::DragFloat3(("Position " + std::to_string(i)).c_str(), &scene.SoundObjects[i].SoundPosition.x);
					ImGui::TreePop();
				}
			}

			ImGui::TreePop();
		}
		ImGui::Spacing();
		if (ImGui::TreeNode("Collider")) {

			ImGui::TreePop();
		}
	}
	ImGui::End();
	//scene
	ImGui::Render(); // Renders the ImGUI elements
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}