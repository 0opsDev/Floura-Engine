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

int Main::VertNum = 0, Main::FragNum = 0;
bool Main::sleepState = true;

unsigned int width = 800, height = 600;

float Main::cameraSettings[3] = { 60.0f, 0.1f, 1000.0f }; // FOV, near, far

bool doPlayerCollision = true;
float PlayerHeight = 1.8f;
float CrouchHighDiff = 0.9f;

float window_width;
float window_height;
glm::vec3 Main::initalCameraPos = glm::vec3(0,0,0);

TimeAccumulator TA3; TimeAccumulator TA2;
Scene scene;
void Main::updateModelLua( 
	std::vector<std::string> path, std::vector<std::string> modelName, std::vector<bool> isCulling,
	std::vector<float> Modelx, std::vector<float> Modely, std::vector<float> Modelz,
	std::vector<float> RotW, std::vector<float> RotX, std::vector<float> RotY, std::vector<float> RotZ,
	std::vector<float> ScaleX, std::vector<float> ScaleY, std::vector<float> ScaleZ
)	
{
	//if (true) { //turn true for debugging
	//	for (size_t i = 0; i < modelName.size(); i++) {
	//		std::cout << "Received modelName: " << modelName[i] << std::endl;
	//		std::cout << "Received Path: " << path[i] << std::endl;
	//	}
	//}
}

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

	// Get the video mode of the primary monitor
	// Get the primary monitor
	GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
	if (!primaryMonitor) { std::cerr << "Failed to get primary monitor" << std::endl; glfwTerminate(); return -1; }

	// Get the video mode of the primary monitor
	const GLFWvidmode* videoMode = glfwGetVideoMode(primaryMonitor);
	if (!videoMode) { std::cerr << "Failed to get video mode" << std::endl; glfwTerminate(); return -1; }

	// fall back values if height*width is null
	if (std::isnan(width)){width = videoMode->width;}
	if (std::isnan(height)){height = videoMode->height;} 

	//    GLFWwindow* window = glfwCreateWindow(videoMode->width, videoMode->height, "Farquhar Engine OPEN GL - 1.3", primaryMonitor, NULL);
	GLFWwindow* window = glfwCreateWindow(videoMode->width, videoMode->height, SettingsUtils::s_WindowTitle.c_str(), NULL, NULL); // create window

	// error checking
	if (window == NULL) { if (init::LogALL || init::LogSystems) std::cout << "failed to create window" << std::endl; glfwTerminate(); return -1; } // "failed to create window"

	glfwMakeContextCurrent(window);	//make window current context

	gladLoadGL(); // load open gl config

	scene.LoadScene(SettingsUtils::sceneName);
	FileClass::loadShaderProgram(Main::VertNum, Main::FragNum, RenderClass::shaderProgram); // feed the shader prog real data << should take a shader file class
	RenderClass::shaderProgram.Activate(); // activate new shader program for use

	// move to framebuffer class
	Shader frameBufferProgram;
	frameBufferProgram.LoadShader("Shaders/PostProcess/framebuffer.vert", "Shaders/PostProcess/framebuffer.frag");

	//area of open gl we want to render in
	//screen assignment after fallback
	ScreenUtils::SetScreenSize(window, width, height);  // set window and viewport w&h
	if (init::LogALL || init::LogSystems) std::cout << "Primary monitor resolution: " << width << "x" << height << std::endl;

	// window logo creation and assignment
	init::initLogo(window, "assets/Icons/Icon.png");

	// INITIALIZE CAMERA
	Camera::InitCamera(width, height, Main::initalCameraPos); 	// camera ratio pos
	//std::cout << initalCameraPos.x << " " << initalCameraPos.y << " " << initalCameraPos.z << std::endl;
	//std::cout << Camera::Position.x << " " << Camera::Position.y << " " << Camera::Position.z << std::endl;
	RenderClass::init(window, width, height);
	// Model Loader

	TempScene::init(); // Initialize scene
	Player::init();

	//Player::feetpos = glm::vec3(Camera::Position.x, (Camera::Position.y - Camera::PlayerHeightCurrent), Camera::Position.z);
	auto stopInitTime = std::chrono::high_resolution_clock::now();
	auto initDuration = std::chrono::duration_cast<std::chrono::microseconds>(stopInitTime - startInitTime);
	if (init::LogALL || init::LogSystems) std::cout << "init Duration: " << initDuration.count() / 1000000.0 << std::endl;

	while (!glfwWindowShouldClose(window)) // GAME LOOP
	{
		//std::cout << RenderClass::CameraXYZ.x << " " << RenderClass::CameraXYZ.y << " " << RenderClass::CameraXYZ.z << std::endl;
		//std::cout << Camera::Position.x << " " << Camera::Position.y << " " << Camera::Position.z << std::endl;
		RenderClass::ClearFramebuffers(); // Clear Framebuffers

		TimeUtil::updateDeltaTime(); // Update delta time
		ScriptRunner::update();
		InputUtil::UpdateCurrentKey(window);
		
		if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS) {
			Camera::s_DoGravity = false;
			doPlayerCollision = false;
		}
		if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS) {
			Camera::s_DoGravity = true;
			doPlayerCollision = true;
		}
		//TA2
		TA2.update();
		// Update FPS and window title every second  
		if (TA2.Counter >= 1 / 10.0f) {
			if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS) { ImGuiCamera::imGuiPanels[0] = !ImGuiCamera::imGuiPanels[0]; }
			//std::cout << "update" << std::endl;
			TA2.reset();
		}

		//Player::feetpos = glm::vec3(Camera::Position.x, (Camera::Position.y - Camera::PlayerHeightCurrent), Camera::Position.z);

		if (Camera::s_DoGravity) {
			//crouch 
			if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) { Camera::PlayerHeightCurrent = CrouchHighDiff;}
			else { Camera::PlayerHeightCurrent = PlayerHeight;}
		}

		if (!Camera::s_DoGravity) { Camera::DoJump = true; };

		// Camera Always Stays Above Feet Position**
		//Camera::Position = glm::vec3(Player::feetpos.x, Player::feetpos.y + Camera::PlayerHeightCurrent, Player::feetpos.z);

		auto startInitTime2 = std::chrono::high_resolution_clock::now();

		scene.Update(); // keep scene above player
		//for (size_t i = 0; i < scene.modelObjects.size(); i++)
		//{
		//	std::cout << scene.modelObjects[i].ObjectName << std::endl;
		//}
		
		TempScene::Update(); // Update scene
		Camera::Inputs(window);
		Player::update();

		RenderClass::Render(window, frameBufferProgram, window_width, window_height, width, height);
		if (ImGuiCamera::imGuiPanels[0]) { Main::imGuiMAIN(window, primaryMonitor); }

		RenderClass::Swapchain(window, frameBufferProgram, primaryMonitor); // tip to self, work down to up (lines)
		
		auto stopInitTime2 = std::chrono::high_resolution_clock::now();
		auto initDuration2 = std::chrono::duration_cast<std::chrono::microseconds>(stopInitTime2 - startInitTime2);
		ImGuiCamera::Render = (initDuration2.count() / 1000.0);
	
		//Player::isGrounded = false;
		Player::isColliding = false;
	}
	// Cleanup: Delete all objects on close
	Main::sleepState = false;

	ImGui_ImplOpenGL3_Shutdown(), ImGui_ImplGlfw_Shutdown(), ImGui::DestroyContext(); // Kill ImGui

	RenderClass::Cleanup();
	//FootSound.DeleteSound();
	frameBufferProgram.Delete();
	Skybox::Delete();
	TempScene::Delete(); // Delete scene
	SoundRunner::Delete();
	scene.Delete();
	//SolidColour.Delete();
	glfwDestroyWindow(window), glfwTerminate(); // Kill opengl
	return 0;
}

void Main::LoadPlayerConfig() {
	// Load PlayerConfig.json
	std::ifstream playerConfigFile("Settings/PlayerController.json");
	if (playerConfigFile.is_open()) {
		json playerConfigData;
		playerConfigFile >> playerConfigData;
		playerConfigFile.close();

		doPlayerCollision = playerConfigData[0]["PlayerCollision"];
		if (init::LogALL || init::LogSystems) std::cout << "Player Collision: " << doPlayerCollision << std::endl;
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

		height = settingsData[0]["Height"];
		width = settingsData[0]["Width"];
		SettingsUtils::tempHeight = settingsData[0]["Height"];
		SettingsUtils::tempWidth = settingsData[0]["Width"];

		ScreenUtils::doVsync = settingsData[0]["Vsync"];
		Main::cameraSettings[0] = settingsData[0]["FOV"];
		SettingsUtils::sceneName = "Scenes/" + settingsData[0]["Scene"].get<std::string>();

		Camera::s_sensitivityY = settingsData[0]["SensitivityY"];
		Camera::s_sensitivityX = settingsData[0]["SensitivityX"];

		ImGuiCamera::imGuiPanels[0] = settingsData[0]["imGui"];

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

		settingsData[0]["Vsync"] = ScreenUtils::doVsync;
		settingsData[0]["FOV"] = Main::cameraSettings[0];

		settingsData[0]["SensitivityY"] = Camera::s_sensitivityY;
		settingsData[0]["SensitivityX"] = Camera::s_sensitivityX;

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

char name[32] = "Name";
char Path[64] = "Assets/";
bool LOD = false;
// Holds ImGui Variables and Windows
void Main::imGuiMAIN(GLFWwindow* window,
	GLFWmonitor* monitorT) {
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
	ImGuiCamera::PanelsWindow();
	// Rendering panel
	if (ImGuiCamera::imGuiPanels[1]) {

		ImGui::Begin("Rendering"); // ImGUI window creation

		//rendering here
		ImGuiCamera::RenderWindow(window, monitorT, width, height);

		ImGuiCamera::ShaderWindow();
		// Lighting panel
		ImGuiCamera::LightWindow();

		ImGui::End();
	}
	// Camera panel
	if (ImGuiCamera::imGuiPanels[2]) {
		ImGui::Begin("Camera Settings"); // ImGUI window creation
		//std::to_string(footCollision)
		ImGui::Text(("Camera Position: x: " + std::to_string(Camera::Position.x) + "y: " + std::to_string(Camera::Position.y) + "z: " + std::to_string(Camera::Position.z)).c_str());
		glm::quat cameraQuat = Camera::Orientation; // Ensure Orientation is a quaternion
		glm::vec3 eulerAngles = glm::degrees(glm::eulerAngles(cameraQuat));
		ImGui::Text(("Camera Rotation: Yaw: " + std::to_string(eulerAngles.x) + " Pitch: " + std::to_string(eulerAngles.y) + " Roll: " + std::to_string(eulerAngles.z)).c_str());
		ImGui::Spacing();
		if (ImGui::TreeNode("Controls")) {
			ImGui::Text("Transform");
			if (ImGui::SmallButton("Reset Camera")) {
				Camera::Position = glm::vec3(0, 0, 0);
			} // reset cam pos
			ImGui::DragFloat3("Camera Transform", &Camera::Position.x, Camera::Position.y, Camera::Position.z); // set cam pos
			ImGui::DragFloat("Camera Speed", &Camera::s_scrollSpeed); //Camera

			ImGui::Spacing();
			ImGui::Text("Bindings");
			ImGui::DragFloat("Camera Sensitivity X", &Camera::s_sensitivityX);
			ImGui::DragFloat("Camera Sensitivity Y", &Camera::s_sensitivityY);
			ImGui::TreePop();
		}
		ImGui::Spacing();
		if (ImGui::TreeNode("Perspective")) {
			ImGui::Text("View");
			ImGui::SliderFloat("FOV", &Main::cameraSettings[0], 0.1f, 160.0f); //FOV
			ImGui::DragFloat2("Near and Far Plane", &Main::cameraSettings[1], Main::cameraSettings[2]); // Near and FarPlane
			ImGui::TreePop();
		}
		ImGui::Spacing();
		if (ImGui::TreeNode("Collision")) {
			ImGui::Text(("Foot Collision: " + std::to_string(Player::isColliding)).c_str());
			ImGui::Checkbox("doPlayerCollision: ", &doPlayerCollision);
			ImGui::Checkbox("doPlayerBoxCollision: ", &CubeCollider::CollideWithCamera);
			ImGui::TreePop();
		}
		ImGui::Spacing();
		ImGui::Checkbox("DoGravity: ", &Camera::s_DoGravity);

		ImGui::End();
	}

	if (ImGuiCamera::imGuiPanels[3]) {
		ImGuiCamera::viewport();
	}

	if (ImGuiCamera::imGuiPanels[5]) {
		ImGuiCamera::PhysicsWindow();
	}

	if (ImGuiCamera::imGuiPanels[6]) {
		ImGuiCamera::DebugWindow();
	}

	if (ImGuiCamera::imGuiPanels[7]) {
		ImGuiCamera::TextEditor();
	}

	if (ImGuiCamera::imGuiPanels[8]) {
		ImGuiCamera::audio();
	}

	if (ImGuiCamera::imGuiPanels[4]) {
		ImGui::Begin("Scene hierarchy"); // ImGUI window creation
		ImGui::Text("object hierarchy");

		if (ImGui::SmallButton("Save")) {
			scene.SaveScene(SettingsUtils::sceneName);
		}

		if (ImGui::TreeNode("Add New Object")) {
			ImGui::Checkbox("LOD", &LOD);
			ImGui::InputText("Path Input", Path, IM_ARRAYSIZE(Path));
			ImGui::InputText("Name Input", name, IM_ARRAYSIZE(name));

			if (ImGui::SmallButton("Create")) {
				//scene.AddSceneModelObject("Static", "Assets/Models/barrel/barrel2.gltf", ("barrel" + std::to_string(scene.modelObjects.size())));
				if (LOD) scene.AddSceneModelObject("LOD", Path, name);
				else scene.AddSceneModelObject("Static", Path, name);

			}
			ImGui::TreePop();
		}

		for (size_t i = 0; i < scene.modelObjects.size(); i++)
		{
			// would be better if i can select one of the many objects and ill have a properties plane to edit the data instead of this
			// also a add window would be nice for adding things
			if (ImGui::TreeNode((scene.modelObjects[i].ObjectName).c_str())) {
				// position
				ImGui::DragFloat3(("Position " + std::to_string(i)).c_str(), &scene.modelObjects[i].transform.x,
					scene.modelObjects[i].transform.y, scene.modelObjects[i].transform.z);

				// scale
				ImGui::DragFloat3(("Scale " + std::to_string(i)).c_str(), &scene.modelObjects[i].scale.x,
					scene.modelObjects[i].scale.y, scene.modelObjects[i].scale.z);

				if (ImGui::SmallButton("Delete")) {
					scene.modelObjects.erase(scene.modelObjects.begin() + i);
				}

				ImGui::TreePop();
			}
		}
	}
	ImGui::End();
	//scene
	ImGui::Render(); // Renders the ImGUI elements
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}