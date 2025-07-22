#include "Main.h"
#include "Camera/Camera.h"
#include "utils/Init.h"
#include <glm/gtx/string_cast.hpp>
#include "utils/timeUtil.h" 
#include <thread>
#include <chrono>
#include <utils/SettingsUtil.h>
#include "utils/timeAccumulator.h"
#include "utils/InputUtil.h"
#include "Scripting/ScriptRunner.h"
#include "imgui/imgui_impl_opengl3.h"
#include "File/File.h"
#include <UI/ImGui/ImGuiWindow.h>
#include <Sound/SoundRunner.h>
#include "Render.h"
#include "tempscene.h"
#include "scene.h"
#include <Gameplay/Player.h>
#include <Render/window/WindowHandler.h>
#include "ImGuiFileDialog/ImGuiFileDialog.h"

int Main::FragNum = 0;
bool Main::sleepState = true;

float Main::cameraSettings[3] = { 60.0f, 0.1f, 1000.0f }; // FOV, near, far // move this to camera class or something

TimeAccumulator TA2;

//Main Function
int main() // global variables do not work with threads
{
	Main::sleepState = true;
	auto startInitTime = std::chrono::high_resolution_clock::now();
	init::initLog();// init logs (should always be before priniting anything)
	init::initGLFW(); // initialize glfw
	Main::loadSettings();
	//Main::loadEngineSettings();
	ScriptRunner::init(SettingsUtils::sceneName + "/LuaStartup.json");
	SoundRunner::init();

	windowHandler::InitMainWidnow();

	gladLoadGL(); // load open gl config

	Scene::LoadScene(SettingsUtils::sceneName);
	FileClass::loadShaderProgram(0, Main::FragNum, RenderClass::shaderProgram); // feed the shader prog real data << should take a shader file class
	RenderClass::shaderProgram.Activate(); // activate new shader program for use

	// INITIALIZE CAMERA
	Camera::InitCamera(windowHandler::width, windowHandler::height, Scene::initalCameraPos); 	// camera ratio pos
	RenderClass::init(windowHandler::window, windowHandler::width, windowHandler::height);
	Scene::init();
	// Model Loader
	
	//just a class to test stuff
	TempScene::init(); // Initialize scene

	Player::init();

	if (ImGuiWindow::imGuiEnabled) {
		ImGuiWindow::init();
	}

	//Player::feetpos = glm::vec3(Camera::Position.x, (Camera::Position.y - Camera::PlayerHeightCurrent), Camera::Position.z);
	auto stopInitTime = std::chrono::high_resolution_clock::now();
	auto initDuration = std::chrono::duration_cast<std::chrono::microseconds>(stopInitTime - startInitTime);
	if (init::LogALL || init::LogSystems) std::cout << "init Duration: " << initDuration.count() / 1000000.0 << std::endl;

	while (!glfwWindowShouldClose(windowHandler::window)) // GAME LOOP
	{
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

		if (!Camera::s_DoGravity) { Camera::DoJump = true; };

		auto startInitTime2 = std::chrono::high_resolution_clock::now();

		Scene::Update(); // keep scene above player

		TempScene::Update(); // Update scene
		Camera::Inputs(windowHandler::window);
		Player::update();

		RenderClass::Render(windowHandler::window, windowHandler::width, windowHandler::height);
		if (ImGuiWindow::imGuiPanels[0] && ImGuiWindow::imGuiEnabled) { Main::imGuiMAIN(windowHandler::window); }

		RenderClass::Swapchain(windowHandler::window); // tip to self, work down to up (lines)

		auto stopInitTime2 = std::chrono::high_resolution_clock::now();
		auto initDuration2 = std::chrono::duration_cast<std::chrono::microseconds>(stopInitTime2 - startInitTime2);
		ImGuiWindow::Render = (initDuration2.count() / 1000.0);

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
		//Main::cameraSettings[0] = settingsData[0]["FOV"];
		SettingsUtils::sceneName = "Scenes/" + settingsData[0]["Scene"].get<std::string>();

		ImGuiWindow::imGuiEnabled = settingsData[0]["imGui"];

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

bool addContentBool = false;
// Holds ImGui Variables and Windows
void Main::imGuiMAIN(GLFWwindow* window) {
	//Tell Imgui a new frame is about to begin
	ImGui_ImplOpenGL3_NewFrame(); ImGui_ImplGlfw_NewFrame(); ImGui::NewFrame();

	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImGui::GetMainViewport()->Size);

	// Important flags for the main dockspace window
	ImGuiWindowFlags window_flags =
		ImGuiWindowFlags_MenuBar |
		ImGuiWindowFlags_NoDocking |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoNavFocus;

	// Begin the main docking window
	ImGui::Begin("Docking Window", nullptr, window_flags);

	if (ImGui::BeginMenuBar())
	{
		ImGui::Image(
			(ImTextureID)(intptr_t)ImGuiWindow::logoIcon.ID,
			ImVec2(20, 20)
		);

		if (ImGui::BeginMenu("File"))
		{
			ImGui::MenuItem("New Scene");
			if (ImGui::MenuItem("Open Scene")) {
				IGFD::FileDialogConfig config;
				config.path = ".";
				ImGuiFileDialog::Instance()->OpenDialog("LoadScene", "Choose Scene", ".*", config);
			}

			ImGui::MenuItem("Save Scene As");
			if (ImGui::MenuItem("Save Scene")) Scene::SaveScene(SettingsUtils::sceneName);
			if (ImGui::MenuItem("Reload Scene")) Scene::LoadScene(SettingsUtils::sceneName);

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View"))
		{
			ImGui::Checkbox("Wireframe", &ImGuiWindow::isWireframe);
			ImGui::Checkbox("showBoxCollider", &CubeCollider::showBoxCollider);
			ImGui::Checkbox("showViewportIcons", &ImGuiWindow::showViewportIcons);

			//if (ImGui::MenuItem("Wireframe")) {
			//	// Toggle some view state
			//}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Reload Shaders?")) RenderClass::initGlobalShaders();
			ImGui::Text("Scripts:");
			if (ImGui::SmallButton("Stop")) { ScriptRunner::clearScripts(); } // save settings button
			if (ImGui::SmallButton("Start")) { ScriptRunner::init(SettingsUtils::sceneName + "LuaStartup.json"); } // save settings button
			if (ImGui::SmallButton("Restart")) { ScriptRunner::clearScripts(); ScriptRunner::init(SettingsUtils::sceneName + "LuaStartup.json"); } // save settings button
			ImGui::Spacing();
			ImGui::Text("Volume");
			ImGui::SliderFloat("Global Volume", &SoundRunner::GlobalVolume, 0, 1);
			ImGui::SliderFloat("Music Volume", &SoundRunner::MusicVolume, 0, 1);
			ImGui::SliderFloat("Environment Volume", &SoundRunner::environmentVolume, 0, 1);
			ImGui::SliderFloat("Entity Volume", &SoundRunner::entityVolume, 0, 1);

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Panels")) {
			//panel
			ImGuiWindow::PanelsWindow();
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Debug")) {
			ImGui::Checkbox("DoDeferredLightingPass", &RenderClass::DoDeferredLightingPass);
			ImGui::Checkbox("DoForwardLightingPass", &RenderClass::DoForwardLightingPass);
			ImGui::Checkbox("DoComputeLightingPass", &RenderClass::DoComputeLightingPass);
			ImGui::Checkbox("doPlayerBoxCollision: ", &CubeCollider::CollideWithCamera);
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	// display
	if (ImGuiFileDialog::Instance()->Display("LoadScene")) {
		if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
			std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
			std::replace(filePath.begin(), filePath.end(), '\\', '/');
			SettingsUtils::sceneName = filePath;
			std::cout << filePath << std::endl;
			Scene::LoadScene(filePath);
		}
		ImGuiFileDialog::Instance()->Close();
	}

	ImGuiID dockspace_id = ImGui::GetID("MainDockspace");
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

	ImGui::End();


	// Rendering panel
	if (ImGuiWindow::imGuiPanels[1]) {

		ImGui::Begin("Rendering"); // ImGUI window creation

		//rendering here
		ImGuiWindow::RenderWindow(window, windowHandler::width, windowHandler::height);

		ImGuiWindow::ShaderWindow();

		ImGui::ColorEdit3("fog RGBA", &RenderClass::fogRGBA.r);	// sky and light

		if (ImGui::TreeNode("Light Settings")) {

			// cone settings
			ImGui::Text("cone size");
			ImGui::SliderFloat("cone Size (D: 0.95)", &RenderClass::ConeSI[1], 0.0f, 1.0f);
			ImGui::SliderFloat("cone Strength (D: 0.05)", &RenderClass::ConeSI[0], 0.0f, 0.90f);

			ImGui::Text("Light Angle");
			ImGui::DragFloat3("Cone Angle", RenderClass::ConeRot);

			ImGui::TreePop();
		}

		ImGui::End();
	}

	if (ImGuiWindow::imGuiPanels[3]) {
		ImGuiWindow::viewport();
	}

	if (ImGuiWindow::imGuiPanels[6]) {
		ImGuiWindow::PreformanceProfiler();
	}

	if (ImGuiWindow::imGuiPanels[7]) {
		ImGuiWindow::TextEditor();
	}

	if (ImGuiWindow::imGuiPanels[4]) {
		ImGui::Begin("Scene hierarchy"); // ImGUI window creation
		if (ImGui::ImageButton("##SaveIcon", (ImTextureID)ImGuiWindow::SaveIcon.ID, ImVec2(30, 30))) {
			Scene::SaveScene(SettingsUtils::sceneName);
		}
		ImGui::SameLine();
		if (ImGui::ImageButton("##arrowIcon", (ImTextureID)ImGuiWindow::arrowIcon.ID, ImVec2(30, 30))) {
			Scene::LoadScene(SettingsUtils::sceneName);
		}
		ImGui::SameLine();
		if (ImGui::ImageButton("##crossIcon", (ImTextureID)ImGuiWindow::crossIcon.ID, ImVec2(30, 30))) {
			Scene::Delete();
		}
		ImGui::Spacing();


		//ImGuiWindow::SelectedObjectType == "Camera"
		ImGuiWindow::create();

		ImGuiWindow::HierarchyList();
		ImGui::End();
	}

	if (ImGuiWindow::imGuiPanels[9]) {
		ImGui::Begin("Inspector"); // ImGUI window creation
		ImGui::Text("Inspector");
		ImGui::Text(("Selected Object Type : " + ImGuiWindow::SelectedObjectType).c_str());
		ImGui::Text(("Index: " + std::to_string(ImGuiWindow::SelectedObjectIndex)).c_str());

		if (ImGuiWindow::SelectedObjectType == "Camera") {
			ImGuiWindow::CameraWindow();
		}

		if (ImGuiWindow::SelectedObjectType == "Model") {
			ImGuiWindow::ModelWindow();
		}
	
		if (ImGuiWindow::SelectedObjectType == "BillBoard") {
			ImGuiWindow::BillBoardWindow();
		}

		if (ImGuiWindow::SelectedObjectType == "Sound") {
		}

		if (ImGuiWindow::SelectedObjectType == "Collider") {
			ImGuiWindow::ColliderWindow();
		}
		//std::cout << Scene::enabled.size() << std::endl;
		if (ImGuiWindow::SelectedObjectType == "Light") {
			ImGuiWindow::LightWindow();
		}

		if (ImGuiWindow::SelectedObjectType == "DirectLight"){
		
		}

		if (ImGuiWindow::SelectedObjectType == "Skybox") {
			ImGuiWindow::SkyBoxWindow();
		}
		ImGui::End();
	}
	
	if (ImGuiWindow::imGuiPanels[10]) {
		ImGui::Begin("Content Folder"); // ImGUI window creation
		if (ImGui::ImageButton("##plusIcon", (ImTextureID)ImGuiWindow::plusIcon.ID, ImVec2(20, 20))) {
			addContentBool = true;
		}
		if (addContentBool) {
			ImGuiWindow::addWindow("content", addContentBool);
		}
		for (size_t i = 0; i < ImGuiWindow::ContentObjects.size(); i++)
		{
			ImGui::BeginGroup();
			if (ImGuiWindow::ContentObjects[i] == "Model") {
				if (ImGui::ImageButton(("##ObjectIcon" + std::to_string(i)).c_str(), (ImTextureID)ImGuiWindow::ModelIcon.ID, ImVec2(100, 100))) {
					Scene::AddSceneModelObject(ImGuiWindow::ContentObjectTypes[i], ImGuiWindow::ContentObjectPaths[i], ImGuiWindow::ContentObjectNames[i]);
				}
				ImGui::Text((ImGuiWindow::ContentObjectNames[i]).c_str());
				ImGui::SameLine();
				if (ImGui::ImageButton(("##crossIcon" + std::to_string(i)).c_str(), (ImTextureID)ImGuiWindow::crossIcon.ID, ImVec2(10, 10))) {
					ImGuiWindow::ContentObjects.erase(ImGuiWindow::ContentObjects.begin() + i);
					ImGuiWindow::ContentObjectNames.erase(ImGuiWindow::ContentObjectNames.begin() + i);
					ImGuiWindow::ContentObjectPaths.erase(ImGuiWindow::ContentObjectPaths.begin() + i);
					ImGuiWindow::ContentObjectTypes.erase(ImGuiWindow::ContentObjectTypes.begin() + i);
				}
			}
			if (ImGuiWindow::ContentObjects[i] == "BillBoard") {
				if (ImGui::ImageButton(("##BillBoardIcon" + std::to_string(i)).c_str(), (ImTextureID)ImGuiWindow::BillBoardIcon.ID, ImVec2(100, 100))) {
					Scene::AddSceneBillBoardObject(ImGuiWindow::ContentObjectNames[i], ImGuiWindow::ContentObjectTypes[i], ImGuiWindow::ContentObjectPaths[i]);
				}
				ImGui::Text((ImGuiWindow::ContentObjectNames[i]).c_str());
				ImGui::SameLine();
				if (ImGui::ImageButton(("##crossIcon" + std::to_string(i)).c_str(), (ImTextureID)ImGuiWindow::crossIcon.ID, ImVec2(10, 10))) {
					ImGuiWindow::ContentObjects.erase(ImGuiWindow::ContentObjects.begin() + i);
					ImGuiWindow::ContentObjectNames.erase(ImGuiWindow::ContentObjectNames.begin() + i);
					ImGuiWindow::ContentObjectPaths.erase(ImGuiWindow::ContentObjectPaths.begin() + i);
					ImGuiWindow::ContentObjectTypes.erase(ImGuiWindow::ContentObjectTypes.begin() + i);
				}
			}
			ImGui::EndGroup();

			ImGui::SameLine();
		}

		ImGui::End();
	}

	//scene
	ImGui::Render(); // Renders the ImGUI elements
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}