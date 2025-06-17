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
#include "scene.h"

bool anyCollision = false; // Track collision status
int Main::VertNum = 0, Main::FragNum = 0;
bool Main::ApplyShader = true;
bool Main::sleepState = true;

unsigned int width = 800, height = 600;

float Main::cameraSettings[3] = { 60.0f, 0.1f, 1000.0f }; // FOV, near, far

bool doPlayerCollision = true;
float PlayerHeight = 1.8f;
float CrouchHighDiff = 0.9f;

float window_width;
float window_height;

TimeAccumulator TA3; TimeAccumulator TA2;

void Main::updateModelLua( 
	std::vector<std::string> path, std::vector<std::string> modelName, std::vector<bool> isCulling,
	std::vector<float> Modelx, std::vector<float> Modely, std::vector<float> Modelz,
	std::vector<float> RotW, std::vector<float> RotX, std::vector<float> RotY, std::vector<float> RotZ,
	std::vector<float> ScaleX, std::vector<float> ScaleY, std::vector<float> ScaleZ
)	
{
	if (true) { //turn true for debugging
		for (size_t i = 0; i < modelName.size(); i++) {
			std::cout << "Received modelName: " << modelName[i] << std::endl;
			std::cout << "Received Path: " << path[i] << std::endl;
		}
	}
}

//Main Function
int main() // global variables do not work with threads
{
	Main::sleepState = true;
	auto startInitTime = std::chrono::high_resolution_clock::now();
	init::initLog();// init logs (should always be before priniting anything)
	init::initGLFW(); // initialize glfw
	Main::loadSettings();
	Main::loadEngineSettings();
	ScriptRunner::init(SettingsUtils::mapName + "/LuaStartup.json");
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

	FileClass::loadShaderProgram(Main::VertNum, Main::FragNum, RenderClass::shaderProgram);// feed the shader prog real data
	RenderClass::shaderProgram.Activate(); // activate new shader program for use

	// move to framebuffer class
	Shader frameBufferProgram;
	frameBufferProgram.LoadShader("Shaders/PostProcess/framebuffer.vert", "Shaders/PostProcess/framebuffer.frag");
	frameBufferProgram.Activate();
	frameBufferProgram.setInt("screenTexture", 0);

	//area of open gl we want to render in
	//screen assignment after fallback
	ScreenUtils::SetScreenSize(window, width, height);  // set window and viewport w&h
	if (init::LogALL || init::LogSystems) std::cout << "Primary monitor resolution: " << width << "x" << height << std::endl;

	// window logo creation and assignment
	init::initLogo(window, "assets/Icons/Icon.png");

	// INITIALIZE CAMERA
	Camera::InitCamera(width, height, glm::vec3(0.0f, 0.0f, 50.0f)); 	// camera ratio pos
	Camera::Position = RenderClass::CameraXYZ; // camera ratio pos //INIT CAMERA POSITION
	RenderClass::init(window, width, height);
	// Model Loader
	std::vector<std::tuple<Model, int, glm::vec3, glm::vec4, glm::vec3, int>> models = FileClass::loadModelsFromJson(SettingsUtils::mapName + "ModelECSData.json"); // Load models from JSON file 

	Main::LoadPlayerConfig();

	Scene::init(); // Initialize scene

	SoundProgram FootSound; FootSound.CreateSound("Assets/Sounds/Footsteps.wav");
	SoundProgram land; land.CreateSound("Assets/Sounds/land.wav");

	glm::vec3 feetpos = glm::vec3(Camera::Position.x, (Camera::Position.y - Camera::PlayerHeightCurrent), Camera::Position.z);
	auto stopInitTime = std::chrono::high_resolution_clock::now();
	auto initDuration = std::chrono::duration_cast<std::chrono::microseconds>(stopInitTime - startInitTime);
	if (init::LogALL || init::LogSystems) std::cout << "init Duration: " << initDuration.count() / 1000000.0 << std::endl;

	while (!glfwWindowShouldClose(window)) // GAME LOOP
	{
		RenderClass::ClearFramebuffers(); // Clear Framebuffers

		TimeUtil::updateDeltaTime(); float deltaTime = TimeUtil::s_DeltaTime; // Update delta time
		ScriptRunner::update();
		InputUtil::UpdateCurrentKey(window);

		Scene::Update(); // Update scene

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
		
		if (Main::ApplyShader) {
			FileClass::loadShaderProgram(Main::VertNum, Main::FragNum, RenderClass::shaderProgram);
			Main::ApplyShader = false;
		}

		feetpos = glm::vec3(Camera::Position.x, (Camera::Position.y - Camera::PlayerHeightCurrent), Camera::Position.z);

		FootSound.SetSoundPosition(feetpos.x, feetpos.y, feetpos.z);
		FootSound.updateCameraPosition();

		//physics
		if (Camera::s_DoGravity) {
			//crouch 
			if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) { Camera::PlayerHeightCurrent = CrouchHighDiff;}
			else { Camera::PlayerHeightCurrent = PlayerHeight;}
		}

		if (doPlayerCollision) { //testing collisions if touching ground
			anyCollision = false;
			for (auto& modelTuple : models) {
				Model& model = std::get<0>(modelTuple);
				glm::vec3 translation = std::get<2>(modelTuple);
				glm::vec4 rotation = std::get<3>(modelTuple);
				glm::vec3 scale = std::get<4>(modelTuple);
				int isCollider = std::get<5>(modelTuple);
				if (isCollider == 1 && model.checkCollide(feetpos, translation, glm::quat(rotation.x, rotation.y, rotation.z, rotation.w), scale, 2)) {
					anyCollision = true;

					// Get collision triangle vertices
					glm::vec3 v0 = model.lastCollisionFace[0];
					glm::vec3 v1 = model.lastCollisionFace[1];
					glm::vec3 v2 = model.lastCollisionFace[2];

					// Compute triangle normal
					glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

					// Compute slope steepness (angle between normal and vertical axis)
					float slopeSteepness = glm::degrees(glm::acos(glm::clamp(glm::dot(normal, glm::vec3(0, 1, 0)), -1.0f, 1.0f)));

					// Compute surface Y height at feet X/Z using barycentric interpolation
					glm::vec3 edge0 = v1 - v0;
					glm::vec3 edge1 = v2 - v0;
					glm::vec3 vp = glm::vec3(feetpos.x, 0.0f, feetpos.z) - glm::vec3(v0.x, 0.0f, v0.z);

					float d00 = glm::dot(edge0, edge0);
					float d01 = glm::dot(edge0, edge1);
					float d11 = glm::dot(edge1, edge1);
					float d20 = glm::dot(vp, edge0);
					float d21 = glm::dot(vp, edge1);

					float denom = d00 * d11 - d01 * d01;
					float u = (d11 * d20 - d01 * d21) / denom;
					float v = (d00 * d21 - d01 * d20) / denom;

					float detectedSurfaceY = v0.y + u * (v1.y - v0.y) + v * (v2.y - v0.y);

					// Force height correction
					if (feetpos.y < detectedSurfaceY) {
						feetpos.y = detectedSurfaceY; // Ensure player stays above the surface

						// Scale push-back stronger for steeper slopes
						float pushStrength = glm::mix(0.05f, 0.5f, slopeSteepness / 90.0f);
						feetpos.x -= normal.x * pushStrength;
						feetpos.z -= normal.z * pushStrength;
					}
					TA3.reset();
					break; // Exit loop once collision is detected
				}
			}
		}
		// Apply Gravity to feetpos Only If No Collision Occurred
		if (!anyCollision && Camera::s_DoGravity) {
			feetpos.y -= (2) * deltaTime; // Falling behavior
			if (TA3.Counter >= (1 / 2)) { if (FootSound.isPlay) FootSound.StopSound();Camera::DoJump = false; }
			else { Camera::DoJump = true; }

			if ((glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) && !(
				glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)) {
				if (!FootSound.isPlay) {
					FootSound.PlaySound(1);
				}
			}
			else {
				FootSound.StopSound();
			}

			TA3.update();
		}
		if (!Camera::s_DoGravity) { Camera::DoJump = true; };

		// Camera Always Stays Above Feet Position**
		Camera::Position = glm::vec3(feetpos.x, feetpos.y + Camera::PlayerHeightCurrent, feetpos.z);

		auto startInitTime2 = std::chrono::high_resolution_clock::now();

		RenderClass::Render(window, frameBufferProgram, window_width, window_height, glm::vec3(RenderClass::LightTransform1[0], RenderClass::LightTransform1[1], RenderClass::LightTransform1[2]), models);
		if (ImGuiCamera::imGuiPanels[0]) { Main::imGuiMAIN(window, RenderClass::shaderProgram, primaryMonitor); }

		RenderClass::Swapchain(window, frameBufferProgram, primaryMonitor); // tip to self, work down to up (lines)
		
		auto stopInitTime2 = std::chrono::high_resolution_clock::now();
		auto initDuration2 = std::chrono::duration_cast<std::chrono::microseconds>(stopInitTime2 - startInitTime2);
		ImGuiCamera::Render = (initDuration2.count() / 1000.0);
	}
	// Cleanup: Delete all objects on close
	Main::sleepState = false;

	ImGui_ImplOpenGL3_Shutdown(), ImGui_ImplGlfw_Shutdown(), ImGui::DestroyContext(); // Kill ImGui

	RenderClass::Cleanup();
	//FootSound.DeleteSound();
	frameBufferProgram.Delete();
	Skybox::Delete();
	land.DeleteSound();
	land.DeleteSound();	
	Scene::Delete(); // Delete scene
	SoundRunner::Delete();
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
		SettingsUtils::mapName = "Maps/" + settingsData[0]["MAP"].get<std::string>() + "/";

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

void Main::loadEngineSettings() {
	// Load EngineDefault.json
	std::ifstream engineDefaultFile(SettingsUtils::mapName + "Engine.json");
	if (engineDefaultFile.is_open()) {
		json engineDefaultData;
		engineDefaultFile >> engineDefaultData;
		engineDefaultFile.close();

		RenderClass::CameraXYZ.x = engineDefaultData[0]["DefaultCameraPos"][0];
		RenderClass::CameraXYZ.y = engineDefaultData[0]["DefaultCameraPos"][1];
		RenderClass::CameraXYZ.z = engineDefaultData[0]["DefaultCameraPos"][2];

		RenderClass::skyRGBA[0] = engineDefaultData[0]["skyRGBA"][0];
		RenderClass::skyRGBA[1] = engineDefaultData[0]["skyRGBA"][1];
		RenderClass::skyRGBA[2] = engineDefaultData[0]["skyRGBA"][2];

		RenderClass::lightRGBA[0] = engineDefaultData[0]["lightRGBA"][0];
		RenderClass::lightRGBA[1] = engineDefaultData[0]["lightRGBA"][1];
		RenderClass::lightRGBA[2] = engineDefaultData[0]["lightRGBA"][2];

		RenderClass::fogRGBA[0] = engineDefaultData[0]["fogRGBA"][0];
		RenderClass::fogRGBA[1] = engineDefaultData[0]["fogRGBA"][1];
		RenderClass::fogRGBA[2] = engineDefaultData[0]["fogRGBA"][2];

		RenderClass::ConeRot[0] = engineDefaultData[0]["ConeRot"][0];
		RenderClass::ConeRot[1] = engineDefaultData[0]["ConeRot"][1];
		RenderClass::ConeRot[2] = engineDefaultData[0]["ConeRot"][2];

		RenderClass::doReflections = engineDefaultData[0]["doReflections"];
		RenderClass::doFog = engineDefaultData[0]["doFog"];
		Main::VertNum = engineDefaultData[0]["VertNum"];
		Main::FragNum = engineDefaultData[0]["FragNum"];

		Main::cameraSettings[1] = std::stof(engineDefaultData[0]["NearPlane"].get<std::string>());
		Main::cameraSettings[2] = std::stof(engineDefaultData[0]["FarPlane"].get<std::string>());

		RenderClass::DepthDistance = engineDefaultData[0]["DepthDistance"];
		RenderClass::DepthPlane[0] = engineDefaultData[0]["DepthPlane"][0];
		RenderClass::DepthPlane[1] = engineDefaultData[0]["DepthPlane"][1];
		SettingsUtils::s_WindowTitle = engineDefaultData[0]["Window"];
		Skybox::DefaultSkyboxPath = engineDefaultData[0]["DefaultSkyboxPath"];
	}
	else {
		std::cerr << "Failed to open Settings/Default/EngineDefault.json" << std::endl;
	}
}

// Holds ImGui Variables and Windows
void Main::imGuiMAIN(GLFWwindow* window, Shader& shaderProgramT,
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
			ImGui::DragFloat3("Camera Transform", &RenderClass::CameraXYZ.x, RenderClass::CameraXYZ.y, RenderClass::CameraXYZ.z); // set cam pos
			if (ImGui::SmallButton("Set")) { Camera::Position = glm::vec3(RenderClass::CameraXYZ.x, RenderClass::CameraXYZ.y, RenderClass::CameraXYZ.z); } // apply cam pos
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
			ImGui::Text(("Foot Collision: " + std::to_string(anyCollision)).c_str());
			ImGui::Checkbox("doPlayerCollision: ", &doPlayerCollision);
			ImGui::Checkbox("doPlayerBoxCollision: ", &CubeCollider::CollideWithCamera);
			ImGui::TreePop();
		}
		ImGui::Spacing();
		ImGui::Checkbox("DoGravity: ", &Camera::s_DoGravity);

		ImGui::End();
	}

	if (ImGuiCamera::imGuiPanels[3]) {
		ImGui::Begin("ViewPort");
		float window_width = ImGui::GetContentRegionAvail().x;
		float window_height = ImGui::GetContentRegionAvail().y;
		ImGui::Image((ImTextureID)(uintptr_t)Framebuffer::frameBufferTexture2, ImVec2(window_width, window_height), ImVec2(0, 1), ImVec2(1, 0));

		//prevEnableLinearScaling
		ScreenUtils::UpdateViewportResize();
		if (ScreenUtils::isResizing == true) {
			//std::cout << "Resolution scale changed!" << std::endl;
			Framebuffer::updateFrameBufferResolution(window_width, window_height); // Update frame buffer resolution
			glViewport(0, 0, (window_width), (window_height));

			Camera::SetViewportSize(window_width, window_height);
			//std::cout << window_width << " " << camera.width << std::endl;
			//std::cout << window_height << " " << camera.height << std::endl;
		}

		ImGui::End();
	}

	if (ImGuiCamera::imGuiPanels[4]) {
		ImGui::Begin("Tabs");
		if (ImGui::SmallButton("Model")) { ImGuiCamera::FileTabs = "Model"; }
		if (ImGui::SmallButton("Audio")) { ImGuiCamera::FileTabs = "Audio"; }
		if (ImGui::SmallButton("Skybox")) { ImGuiCamera::FileTabs = "Skybox"; }
		if (ImGui::SmallButton("Shader")) { ImGuiCamera::FileTabs = "Shader"; }
		if (ImGui::SmallButton("Script")) { ImGuiCamera::FileTabs = "Script"; }
		if (ImGui::SmallButton("Settings")) { ImGuiCamera::FileTabs = "Settings"; }
		ImGui::End();
		ImGui::Begin("Folder");
		ImGui::Text((ImGuiCamera::FileTabs + ":").c_str()); // find out how to display these two on the same line
		if (ImGui::SmallButton("Add")) {};
		ImGui::End();
		ImGui::Begin("Details");
		ImGui::End();
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
	ImGui::Render(); // Renders the ImGUI elements
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}