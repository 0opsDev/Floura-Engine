#include"Render/Model/Model.h"
#include "Camera/Camera.h"
#include "Main.h"
#include "utils/UF.h"
#include "utils/Init.h"
#include "utils/screenutils.h" 
#include <glm/gtx/string_cast.hpp>
#include "utils/timeUtil.h" 
#include <thread>
#include <chrono>
#include <utils/SettingsUtil.h>
#include "utils/timeAccumulator.h"
#include "utils/InputUtil.h"
//temorary
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <OpenAL/efx.h>
#include <OpenAL/efx-presets.h>
#define STB_PERLIN_IMPLEMENTATION
#include <stb/stb_perlin.h>
#include "Render/Shader/Framebuffer.h"
#include "Scripting/ScriptRunner.h"

// Forward declaration of the function
void updateFrameBufferResolution(unsigned int& frameBufferTexture, unsigned int& RBO, unsigned int& frameBufferTexture2, unsigned int& RBO2, unsigned int width, unsigned int height);

int Main::VertNum = 0, Main::FragNum = 0, Main::TempButton = 0; 

unsigned int FBO2, frameBufferTexture2, RBO2, viewVAO, viewVBO, FBO, frameBufferTexture, RBO; // FBO init

bool enableFB = false; // Change this as needed

char UniformInput[64] = {}; // Zero-initialized buffer
float UniformFloat[3] = {}; // Zero-initialized array

// Global Variables
GLfloat ConeSI[3] = { 0.111f, 0.825f, 2.0f };
GLfloat ConeRot[3] = { 0.0f, -1.0f, 0.0f };
GLfloat LightTransform1[] = { 0.0f, 5.0f, 0.0f };
GLfloat CameraXYZ[3] = { 0.0f, 0.0f, 0.0f }; // Initial camera position
GLfloat lightRGBA[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat skyRGBA[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat fogRGBA[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat DepthDistance = 100.0f;
GLfloat DepthPlane[2] = { 0.1f, 100.0f };

std::array<std::string, 6> facesCubemap;

// Render settings
bool doReflections = true;
bool doFog = true;
bool doVsync = false;
bool isWireframe = false;

// Shader settings
bool Stencil = false;
float stencilSize = 0.009f;
GLfloat stencilColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
float gamma = 2.2f;

float resolutionScale = 1;
unsigned int width = 800, height = 600, ViewPortWidth = 800, ViewPortHeight = 600;
int windowedWidth = 0, windowedHeight = 0, widthI = 0, heightI = 0;
bool isFullscreen = false;

bool Panels[4] = { true, true, true, true}; // ImGui Panels

float cameraSettings[3] = { 60.0f, 0.1f, 1000.0f }; // FOV, near, far

bool doPlayerCollision = true;
bool footCollision = false;
float PlayerHeight = 1.8f;
float CrouchHighDiff = 0.9f;
float PlayerHeightCurrent;

// Define the plane's boundaries
float planeMinX = -5.0f; // Left edge of the plane
float planeMaxX = 5.0f;  // Right edge of the plane
float planeMinZ = -5.0f; // Front edge of the plane
float planeMaxZ = 5.0f;  // Back edge of the plane
float planeY = 0.0f;     // Y-position of the plane

TimeAccumulator TA1; TimeAccumulator TA2; TimeAccumulator TA3;

std::string mapName; // Map loading

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
			if (false) {
				std::cout << "Received Path: " << path[i] << std::endl;
				std::cout << "Received x: " << Modelx[i] << std::endl;
				std::cout << "Received y: " << Modely[i] << std::endl;
				std::cout << "Received z: " << Modelz[i] << std::endl;
				std::cout << "Received RotW: " << RotW[i] << std::endl;
				std::cout << "Received RotX: " << RotX[i] << std::endl;
				std::cout << "Received RotY: " << RotY[i] << std::endl;
				std::cout << "Received RotZ: " << RotZ[i] << std::endl;
				std::cout << "Received ScaleX: " << ScaleX[i] << std::endl;
				std::cout << "Received ScaleY: " << ScaleY[i] << std::endl;
				std::cout << "Received ScaleZ: " << ScaleZ[i] << std::endl;
			}
		}
	}
}

// Function to read a specific line from a file
std::string readLineFromFile(const std::string& filePath, int lineNumber) {
	// Shaders
	std::ifstream file(filePath);
	if (!file.is_open()) {
		throw std::runtime_error("Failed to open file: " + filePath);
	}

	std::string line;
	for (int i = 0; i <= lineNumber; ++i) {
		if (!std::getline(file, line)) {
			throw std::runtime_error("Line number out of range in file: " + filePath);
		}
	}
	return line;
}

std::pair<std::string, std::string> getShaderPaths(int vertIndex, int fragIndex) {
	std::ifstream file("Shaders/ShaderList.json"); // turn into string
	if (!file.is_open()) {
		throw std::runtime_error("Failed to open file: Shaders/ShaderList.json");
	}

	json shaderData;
	file >> shaderData;
	file.close();

	std::string vertPath = shaderData[0]["Vert"].at(vertIndex); //check for paths (strings) in array at number index givin and return it
	std::string fragPath = shaderData[0]["Frag"].at(fragIndex);

	return { vertPath, fragPath };
}

void loadShaderProgram(int VertNum, int FragNum, Shader& shaderProgram) { //shader program switcher

	shaderProgram.Delete();

	try {
		std::pair<std::string, std::string> shaderPaths = getShaderPaths(VertNum, FragNum);
		std::string vertFile = shaderPaths.first;
		std::string fragFile = shaderPaths.second;

		if (init::LogALL || init::LogSystems) std::cout << "Vert: " << vertFile << " Frag: " << fragFile << std::endl;

		shaderProgram = Shader(vertFile.c_str(), fragFile.c_str());
	}
	catch (const std::exception& e) {
		std::cerr << "Error loading shader program: " << e.what() << std::endl;
	}
}

// Function to load models from files
std::vector<std::tuple<Model, int, glm::vec3, glm::quat, glm::vec3>> loadModelsFromJson(const std::string& jsonFilePath) {
	std::vector<std::tuple<Model, int, glm::vec3, glm::quat, glm::vec3>> models;
	std::ifstream file(jsonFilePath);
	if (!file.is_open()) {
		std::cerr << "Failed to open file: " << jsonFilePath << std::endl;
		models.emplace_back(Model("Assets/assets/fallback/model/placeholder/placeholder.gltf"), 0, glm::vec3(0.0f, 0.0f, 0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
		return models;
	}

	json modelData;
	try {
		file >> modelData;
	}
	catch (const std::exception& e) {
		std::cerr << "Error parsing JSON file: " << e.what() << std::endl;
		models.emplace_back(Model("Assets/assets/fallback/model/placeholder/placeholder.gltf"), 0, glm::vec3(0.0f, 0.0f, 0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
		return models;
	}
	file.close();

	try {
		for (const auto& item : modelData) {
			std::string name = item.at("name").get<std::string>();
			std::string path = item.at("path").get<std::string>();
			bool isCulling = item.at("isCulling").get<bool>();
			glm::vec3 location = glm::vec3(item.at("Location")[0], item.at("Location")[1], item.at("Location")[2]);
			glm::quat rotation = glm::quat(item.at("Rotation")[0], item.at("Rotation")[1], item.at("Rotation")[2], item.at("Rotation")[3]);
			glm::vec3 scale = glm::vec3(item.at("Scale")[0], item.at("Scale")[1], item.at("Scale")[2]);
			models.emplace_back(Model(("Assets/assets/" + path).c_str()), isCulling, location, rotation, scale);
			if (init::LogALL || init::LogModel) std::cout << "Loaded model: " << '"' << name << '"' << " from path: " << path << " at location: " << glm::to_string(location) << std::endl;
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Error processing JSON data: " << e.what() << std::endl;
		models.emplace_back(Model("Assets/assets/fallback/model/placeholder/placeholder.gltf"), 0, glm::vec3(0.0f, 0.0f, 0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	}

	return models;
}

void LoadPlayerConfig() {
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

void loadSettings() {
	// Load Settings.json
	std::ifstream settingsFile("Settings/Settings.json");
	if (settingsFile.is_open()) {
		json settingsData;
		settingsFile >> settingsData;
		settingsFile.close();

		heightI = settingsData[0]["Height"];
		widthI = settingsData[0]["Width"];
		width = static_cast<unsigned int>(widthI);
		height = static_cast<unsigned int>(heightI); // cast screenArea from screenAreaI

		doVsync = settingsData[0]["Vsync"];
		cameraSettings[0] = settingsData[0]["FOV"];
		mapName = "Maps/" + settingsData[0]["MAP"].get<std::string>() + "/";

		Camera::s_sensitivityY = settingsData[0]["SensitivityY"];
		Camera::s_sensitivityX = settingsData[0]["SensitivityX"];

		Panels[0] = settingsData[0]["imGui"];

		if (init::LogALL || init::LogSystems) std::cout << "Loaded settings from Settings.json" << std::endl;

	}
	else {
		std::cerr << "Failed to open Settings/Settings.json" << std::endl;
	}
}

void saveSettings() {
	try {
		// Load the settings file
		std::ifstream settingsFile("Settings/Settings.json", std::ios::in);
		if (!settingsFile.is_open()) {
			if (init::LogALL || init::LogSystems) std::cout << "Failed to open Settings/Settings.json" << std::endl;
		}

		json settingsData;
		settingsFile >> settingsData;
		settingsFile.close();

		settingsData[0]["Vsync"] = doVsync;
		settingsData[0]["FOV"] = cameraSettings[0];

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

void loadEngineSettings() {
	// Load EngineDefault.json
	std::ifstream engineDefaultFile(mapName + "Engine.json");
	if (engineDefaultFile.is_open()) {
		json engineDefaultData;
		engineDefaultFile >> engineDefaultData;
		engineDefaultFile.close();

		skyRGBA[0] = engineDefaultData[0]["skyRGBA"][0];
		skyRGBA[1] = engineDefaultData[0]["skyRGBA"][1];
		skyRGBA[2] = engineDefaultData[0]["skyRGBA"][2];

		lightRGBA[0] = engineDefaultData[0]["lightRGBA"][0];
		lightRGBA[1] = engineDefaultData[0]["lightRGBA"][1];
		lightRGBA[2] = engineDefaultData[0]["lightRGBA"][2];

		fogRGBA[0] = engineDefaultData[0]["fogRGBA"][0];
		fogRGBA[1] = engineDefaultData[0]["fogRGBA"][1];
		fogRGBA[2] = engineDefaultData[0]["fogRGBA"][2];

		ConeRot[0] = engineDefaultData[0]["ConeRot"][0];
		ConeRot[1] = engineDefaultData[0]["ConeRot"][1];
		ConeRot[2] = engineDefaultData[0]["ConeRot"][2];

		doReflections = engineDefaultData[0]["doReflections"];
		doFog = engineDefaultData[0]["doFog"];
		Main::VertNum = engineDefaultData[0]["VertNum"];
		Main::FragNum = engineDefaultData[0]["FragNum"];

		cameraSettings[1] = std::stof(engineDefaultData[0]["NearPlane"].get<std::string>());
		cameraSettings[2] = std::stof(engineDefaultData[0]["FarPlane"].get<std::string>());

		DepthDistance = engineDefaultData[0]["DepthDistance"];
		DepthPlane[0] = engineDefaultData[0]["DepthPlane"][0];
		DepthPlane[1] = engineDefaultData[0]["DepthPlane"][1];
		SettingsUtils::s_WindowTitle = engineDefaultData[0]["Window"];
	}
	else {
		std::cerr << "Failed to open Settings/Default/EngineDefault.json" << std::endl;
	}
}

void LoadSkybox() {
	std::ifstream SkyboxJson(mapName + "Skybox.json");
	if (SkyboxJson.is_open()) {
		json SkyboxJsonData;
		SkyboxJson >> SkyboxJsonData;
		SkyboxJson.close();

		std::string Path = SkyboxJsonData[0]["Path"].get<std::string>() + "/";

		if (init::LogALL || init::LogModel) std::cout << "Skybox Path: " << Path << std::endl;

		for (int i = 0; i < 6; i++)
		{
			facesCubemap[i] = Path + SkyboxJsonData[0]["Faces"][i].get<std::string>();
			if (init::LogALL || init::LogModel) std::cout << "Skybox Face: " << facesCubemap[i] << std::endl;
		}
	}
	else {
		std::cerr << "Failed to open Skybox.json" << std::endl;
	}
}

// Holds ImGui Variables and Windows
void imGuiMAIN(GLFWwindow* window, Shader shaderProgramT, GLFWmonitor* monitorT, Camera camera, unsigned int& frameBufferTexture, unsigned int& RBO, unsigned int& FBO, unsigned int& frameBufferTexture2) {
	//Tell Imgui a new frame is about to begin
	ImGui_ImplOpenGL3_NewFrame(); ImGui_ImplGlfw_NewFrame(); ImGui::NewFrame();
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(static_cast<unsigned int>(widthI), static_cast<unsigned int>(heightI)));
	ImGui::Begin("Window"); // ImGUI window creation

	ImGui::End();
	//Main Panel
	ImGui::Begin("Panels"); // ImGUI window creation

	ImGui::Text("Settings (Press escape to use mouse)");
	if (ImGui::SmallButton("load")) { loadSettings(); loadEngineSettings(); } // load settings button
	if (ImGui::SmallButton("save (just settings)")) { saveSettings(); } // save settings button
	ImGui::Checkbox("Rendering", &Panels[1]);
	ImGui::Checkbox("Camera Settings", &Panels[2]);
	ImGui::Checkbox("ViewPort", &Panels[3]);
	//Panels
	// Toggle ImGui Windows
	// Rendering panel
	if (Panels[1]) {

		ImGui::Begin("Rendering"); // ImGUI window creation

		if (ImGui::TreeNode("System Infomation")) {
			const GLubyte* version = glGetString(GL_VERSION);
			const GLubyte* renderer = glGetString(GL_RENDERER);

			ImGui::Text("OpenGL Version: %s", version); // Display OpenGL version
			ImGui::Text("Renderer: %s", renderer);  // Display GPU renderer

			ImGui::Text( (std::string("ViewportSize: ") + std::to_string(static_cast<int>(ViewPortWidth * resolutionScale)) + "*" + std::to_string(static_cast<int>(ViewPortHeight * resolutionScale)) ).c_str() );

			ImGui::Spacing();

			static float framerateValues[60] = { 0 };
			static int frValues_offset = 0;
			framerateValues[frValues_offset] = static_cast<float>(TimeUtil::s_frameRate);
			frValues_offset = (frValues_offset + 1) % IM_ARRAYSIZE(framerateValues);

			//Frame time graph
			static float frameTimeValues[90] = { 0 }; //stores 90 snapshots of frametime

			static int ftValues_offset = 0;
			frameTimeValues[ftValues_offset] = TimeUtil::s_DeltaTime * 1000.0f; // Convert to milliseconds
			ftValues_offset = (ftValues_offset + 1) % IM_ARRAYSIZE(frameTimeValues);
			std::string frametimes = "LAT: " + std::to_string(frameTimeValues[ftValues_offset] = TimeUtil::s_DeltaTime * 1000.0f) + " ms";

			ImGui::Text( ("fps: " + std::to_string( static_cast<int>(TimeUtil::s_frameRate1hz) ) ).c_str() );
			ImGui::Text(frametimes.c_str());
			ImGui::Spacing();

			//std::string stringFPS = "FPS: " + std::to_string(deltaTimeStr.frameRate1IHZ) + frametimes;
			if (ImGui::TreeNode("FPS Graph"))
			{
				ImGui::PlotLines("Framerate (FPS) Graph", framerateValues, (IM_ARRAYSIZE(framerateValues)), frValues_offset, nullptr, 0.0f, TimeUtil::s_frameRate * 1.5f, ImVec2(0, 80));
				ImGui::PlotLines("Frame Times (ms) Graph", frameTimeValues, IM_ARRAYSIZE(frameTimeValues), ftValues_offset, nullptr, 0.0f, 50.0f, ImVec2(0, 80));

				ImGui::TreePop();// Ends The ImGui Window
			}

		ImGui::TreePop();// Ends The ImGui Window
		}

		ImGui::Checkbox("isWireframe", &isWireframe);

		ImGui::Dummy(ImVec2(0.0f, 5.0f)); // Adds 5 pixels of vertical space
		if (ImGui::TreeNode("Framerate And Resolution")) {
			ImGui::Text("Framerate Limiters");
			ImGui::Checkbox("Vsync", &doVsync); // Set the value of doVsync (bool)
			// Screen
			ImGui::DragInt("Width", &widthI);
			ImGui::DragInt("Height", &heightI); // screen slider

			ImGui::SliderFloat("Resolution Scale", &resolutionScale, 0.009, 1);
			ImGui::Checkbox("Enable FB shader", &enableFB); // Set the value of doVsync (bool)

			if (ImGui::SmallButton("Apply Changes?")) { // apply button
				width = static_cast<unsigned int>(widthI);
				height = static_cast<unsigned int>(heightI); // cast screenArea from screenAreaI
				glViewport(0, 0, width * resolutionScale, height * resolutionScale); // real internal res
				glfwSetWindowSize(window, width, height);
				//ScreenUtils::SetScreenSize(window, width, height); // set window and viewport w&h
				ScreenUtils::setVSync(doVsync); // Set Vsync to value of doVsync (bool)
				updateFrameBufferResolution(frameBufferTexture, RBO, frameBufferTexture2, RBO2, width, height); // Update frame buffer resolution
			}
			if (ImGui::SmallButton("Toggle Fullscreen (WARNING WILL TOGGLE HDR OFF)"))
			{
				ScreenUtils::toggleFullscreen(window, monitorT, isFullscreen, windowedWidth, windowedHeight);
			} //Toggle Fullscreen


			ImGui::TreePop();// Ends The ImGui Window
		}

		if (ImGui::TreeNode("Shaders")) {
			//Optimisation And Shaders
			ImGui::Checkbox("Enable Stencil Buffer", &Stencil);
			ImGui::DragFloat("Stencil Size", &stencilSize);
			ImGui::DragInt("Shader Number (Vert)", &Main::VertNum);
			ImGui::DragInt("Shader Number (Frag)", &Main::FragNum); // Shader Switching
			if (ImGui::SmallButton("Apply Shader?")) { Main::TempButton = -1; } // apply shader
			ImGui::DragFloat("Gamma", &gamma);
			ImGui::Checkbox("doReflections", &doReflections);
			ImGui::Checkbox("doFog", &doFog); 		//Toggles

			ImGui::Text("DepthBuffer Settings (FOG)");
			ImGui::DragFloat("Depth Distance (FOG)", &DepthDistance);
			ImGui::DragFloat2("Near and Far Depth Plane", DepthPlane);

			ImGui::InputText("Uniform Input", UniformInput, IM_ARRAYSIZE(UniformInput));
			ImGui::DragFloat("UniformFloat", UniformFloat);
			if (false & UniformInput != NULL) { // Debug
				if (init::LogALL || init::LogSystems) std::cout << UniformInput << std::endl;
			}
			ImGui::TreePop();// Ends The ImGui Window
		}
		// Lighting panel

		if (ImGui::TreeNode("Lighting")) {

			if (ImGui::TreeNode("Colour")) {
				ImGui::ColorEdit4("sky RGBA", skyRGBA);
				ImGui::ColorEdit4("light RGBA", lightRGBA);
				ImGui::ColorEdit4("fog RGBA", fogRGBA);	// sky and light
				ImGui::ColorEdit4("Stencil RGBA", stencilColor);
				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Light Settings")) {
				ImGui::DragFloat3("Light Transform", LightTransform1);
				ImGui::DragFloat("light I", &ConeSI[2]);

				// cone settings
				ImGui::Text("cone size");
				ImGui::SliderFloat("cone Size (D: 0.95)", &ConeSI[1], 0.0f, 1.0f);
				ImGui::SliderFloat("cone Strength (D: 0.05)", &ConeSI[0], 0.0f, 0.90f);

				ImGui::Text("Light Angle");
				ImGui::DragFloat3("Cone Angle", ConeRot);

				ImGui::TreePop();
			}

			ImGui::TreePop();// Ends The ImGui Window
		}
		ImGui::End();
	}
	// Camera panel
	if (Panels[2]) {
		ImGui::Begin("Camera Settings"); // ImGUI window creation
		if (ImGui::TreeNode("Controls")) {
			ImGui::Text("Transform");
			if (ImGui::SmallButton("Reset Camera")) { Main::TempButton = 1; } // reset cam pos
			ImGui::DragFloat3("Camera Transform", CameraXYZ); // set cam pos
			if (ImGui::SmallButton("Set")) { Main::TempButton = 2; } // apply cam pos
			ImGui::DragFloat("Camera Speed", &Camera::s_scrollSpeed); //Camera
			
			ImGui::Spacing();
			ImGui::Text("Bindings");
			ImGui::DragFloat("Camera Sensitivity X", &Camera::s_sensitivityX);
			ImGui::DragFloat("Camera Sensitivity Y", &Camera::s_sensitivityY);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Perspective")) {
			ImGui::Spacing();
			ImGui::Text("View");
			ImGui::SliderFloat("FOV", &cameraSettings[0], 0.1f, 160.0f); //FOV
			ImGui::DragFloat2("Near and Far Plane", &cameraSettings[1], cameraSettings[2]); // Near and FarPlane
			ImGui::TreePop();
		}
		ImGui::Spacing();
		ImGui::Checkbox("DoGravity: ", &Camera::s_DoGravity);

		/*
			doPlayerCollision = true;
		*/

		ImGui::End();
	}

	if (Panels[3]) {
		ImGui::Begin("ViewPort");
		const float window_width = ImGui::GetContentRegionAvail().x;
		const float window_height = ImGui::GetContentRegionAvail().y;
		updateFrameBufferResolution(frameBufferTexture, RBO, frameBufferTexture2, RBO2, window_width, window_height); // Update frame buffer resolution
		glViewport(0, 0, (window_width * resolutionScale), (window_height * resolutionScale));
		ImGui::Image((ImTextureID)(uintptr_t)frameBufferTexture2, ImVec2(window_width, window_height), ImVec2(0, 1), ImVec2(1, 0));
		ImGui::End();
	}


	ImGui::End();
	ImGui::Render(); // Renders the ImGUI elements
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
// Holds DeltaTime Based Variables and Functions
void DeltaMain(GLFWwindow* window) {
	// Framerate tracking  

	TA1.update();

	// Update FPS and window title every second  
	if (TA1.Counter >= 1.0f) {
		glfwSetWindowTitle(window, (SettingsUtils::s_WindowTitle + " (FPS: " + std::to_string(static_cast<int>(TimeUtil::s_frameRate1hz) ) +
			" ) (at pos: " + std::to_string(Camera::s_PositionMatrix.x) + " " + std::to_string(Camera::s_PositionMatrix.y) + " " + std::to_string(Camera::s_PositionMatrix.z) +
			") (Foot Collision: " + std::to_string(footCollision) + ")" + " (Title updates at 1hz) ").c_str());

		TA1.reset();
		//timeAccumulator[0] = 0;
	}

	TA2.update();
	if (TA2.Counter >= 1 / 60) {
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_5) == GLFW_PRESS) cameraSettings[0] = std::min(cameraSettings[0] + 0.4f, 160.0f);

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_4) == GLFW_PRESS) cameraSettings[0] = std::max(cameraSettings[0] - 0.4f, 0.1f);
		
		//if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS) { isWireframe = !isWireframe; }
		TA2.reset();
	}
}

void updateFrameBufferResolution(unsigned int& frameBufferTexture, unsigned int& RBO, unsigned int& frameBufferTexture2, unsigned int& RBO2, unsigned int width, unsigned int height) {

	ViewPortWidth = width;
	ViewPortHeight = height;
		// Update first frame buffer texture
		glBindTexture(GL_TEXTURE_2D, frameBufferTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (width * resolutionScale), (height * resolutionScale), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glBindTexture(GL_TEXTURE_2D, 0);

		// Update first render buffer storage
		glBindRenderbuffer(GL_RENDERBUFFER, RBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, (width * resolutionScale), (height * resolutionScale));
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		// Update second frame buffer texture
		glBindTexture(GL_TEXTURE_2D, frameBufferTexture2);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (width * resolutionScale), (height * resolutionScale), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glBindTexture(GL_TEXTURE_2D, 0);

		// Update second render buffer storage
		glBindRenderbuffer(GL_RENDERBUFFER, RBO2);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, (width * resolutionScale), (height * resolutionScale));
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void skyboxBuffer(unsigned int& skyboxVAO, unsigned int& skyboxVBO, unsigned int& skyboxEBO) {
	// Create VAO, VBO, and EBO for the skybox
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glGenBuffers(1, &skyboxEBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(SettingsUtils::s_skyboxVertices), &SettingsUtils::s_skyboxVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(SettingsUtils::s_skyboxIndices), &SettingsUtils::s_skyboxIndices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void cubeboxTexture(unsigned int& cubemapTexture) {
	// Creates the cubemap texture object
	glGenTextures(1, &cubemapTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// These are very important to prevent seams
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	// Cycles through all the textures and attaches them to the cubemap object
	for (unsigned int i = 0; i < 6; i++)
	{
		int width, height, nrChannels;
		unsigned char* data = stbi_load(facesCubemap[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			stbi_set_flip_vertically_on_load(false);
			glTexImage2D
			(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0,
				GL_RGBA,
				width,
				height,
				0,
				GL_RGBA,
				GL_UNSIGNED_BYTE,
				data
			);
			stbi_image_free(data);
		}
		else
		{
			if (init::LogALL || init::LogModel) std::cout << "Failed to load texture: " << facesCubemap[i] << std::endl;
			stbi_image_free(data);
		}
	}
}

//Main Function
int main()
{
	auto startInitTime = std::chrono::high_resolution_clock::now();
	init::initLog();// init logs (should always be before priniting anything)
	ScriptRunner::init();
	init::initGLFW(); // initialize glfw
	std::thread storageThread1(loadSettings);
	// Get the video mode of the primary monitor
	// Get the primary monitor
	GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
	if (!primaryMonitor) { std::cerr << "Failed to get primary monitor" << std::endl; glfwTerminate(); return -1; }

	// Get the video mode of the primary monitor
	const GLFWvidmode* videoMode = glfwGetVideoMode(primaryMonitor);
	if (!videoMode) { std::cerr << "Failed to get video mode" << std::endl; glfwTerminate(); return -1; }

	// second fallback
	// Store the width and height in the test array
	width = videoMode->width;
	height = videoMode->height;

	// Now call glfwGetMonitorPos with correct arguments
	glfwGetMonitorPos(glfwGetPrimaryMonitor(), &widthI, &heightI);

	//    GLFWwindow* window = glfwCreateWindow(videoMode->width, videoMode->height, "Farquhar Engine OPEN GL - 1.3", primaryMonitor, NULL);
	GLFWwindow* window = glfwCreateWindow(videoMode->width, videoMode->height, SettingsUtils::s_WindowTitle.c_str(), NULL, NULL); // create window

	// error checking
	if (window == NULL) { if (init::LogALL || init::LogSystems) std::cout << "failed to create window" << std::endl; glfwTerminate(); return -1; } // "failed to create window"

	glfwMakeContextCurrent(window);	//make window current context

	gladLoadGL(); // load open gl config

	// Enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//area of open gl we want to render in
	//screen assignment after fallback
	ScreenUtils::SetScreenSize(window, width, height);  // set window and viewport w&h
	if (init::LogALL || init::LogSystems) std::cout << "Primary monitor resolution: " << width << "x" << height << std::endl;
	// window logo creation and assignment
	init::initLogo(window);
	ScreenUtils::setVSync(doVsync); // Set Vsync to value of doVsync (bool)
	std::thread storageThread2(loadEngineSettings);

	// shaderprog init
	Shader shaderProgram("Shaders/Empty.shader", "Shaders/Empty.shader"); // create a shader program and feed it Dummy shader and vertex files
	shaderProgram.Delete(); // clean the shader prog for memory management


	loadShaderProgram(Main::VertNum, Main::FragNum, shaderProgram);// feed the shader prog real data
	shaderProgram.Activate(); // activate new shader program for use

	Shader outlineShaderProgram("Shaders/PostProcess/outlining.vert", "Shaders/PostProcess/outlining.frag");
	Shader LightProgram("Shaders/Lighting/light.vert", "Shaders/Lighting/light.frag");
	Shader skyboxShader("Shaders/Skybox/skybox.vert", "Shaders/Skybox/skybox.frag");
	Shader SolidColour("Shaders/Lighting/Default.vert", "Shaders/Db/solidColour.frag");
	skyboxShader.Activate();
	glUniform1i(glGetUniformLocation(skyboxShader.ID, "skybox"), 0);

	Shader frameBufferProgram("Shaders/PostProcess/framebuffer.vert", "Shaders/PostProcess/framebuffer.frag");
	frameBufferProgram.Activate();
	UF::Int(frameBufferProgram.ID, "screenTexture", 0);

	// glenables
	// depth pass. render things in correct order. eg sky behind wall, dirt under water, not random order
	init::initGLenable(false); //bool for direction of polys

	// INITIALIZE CAMERA
	Camera camera(width, height, glm::vec3(0.0f, 0.0f, 50.0f)); 	// camera ratio pos
	camera.Position = glm::vec3(CameraXYZ[0], CameraXYZ[1], CameraXYZ[2]); // camera ratio pos //INIT CAMERA POSITION

	/*
	loading models from json is very taxing
	consider putting this in a thread
	*/

	// Create VAO, VBO, and EBO for the skybox
	unsigned int skyboxVAO, skyboxVBO, skyboxEBO;
	skyboxBuffer(skyboxVAO, skyboxVBO, skyboxEBO);

	LoadSkybox();
	// Create and load a cubemap texture
	unsigned int cubemapTexture;
	cubeboxTexture(cubemapTexture);

	Framebuffer fb1;
	
	fb1.setupMainFBO(viewVAO, viewVBO, FBO, frameBufferTexture, RBO, width, height, SettingsUtils::s_ViewportVerticies);
	fb1.setupSecondFBO(FBO2, frameBufferTexture2, RBO2, width, height);

	init::initImGui(window); // Initialize ImGUI

	// Model Loader
	std::vector<std::tuple<Model, int, glm::vec3, glm::quat, glm::vec3>> models = loadModelsFromJson(mapName + "ModelECSData.json"); // Load models from JSON file 
	Model Lightmodel = "Assets/assets/Light/light.gltf";

	storageThread1.join();
	storageThread2.join();

	LoadPlayerConfig();

	auto stopInitTime = std::chrono::high_resolution_clock::now();
	auto initDuration = std::chrono::duration_cast<std::chrono::microseconds>(stopInitTime - startInitTime);
	if (init::LogALL || init::LogSystems) std::cout << "init Duration: " << initDuration.count() / 1000000.0 << std::endl;
	while (!glfwWindowShouldClose(window)) // GAME LOOP
	{
		TimeUtil::updateDeltaTime(); float deltaTime = TimeUtil::s_DeltaTime; // Update delta time
		DeltaMain(window); // Calls the DeltaMain Method that Handles variables that require delta time (FrameTime, FPS, ETC)
		InputUtil::UpdateCurrentKey(window);

		if (glfwGetKey(window, GLFW_KEY_BACKSLASH) == GLFW_PRESS) { ScriptRunner::clearScripts(); ScriptRunner::init(); };
		ScriptRunner::update(); 

		glm::vec3 cameraPos = Camera::s_PositionMatrix;
		glm::vec3 feetpos = glm::vec3(Camera::s_PositionMatrix.x, (Camera::s_PositionMatrix.y - PlayerHeightCurrent), Camera::s_PositionMatrix.z);
		if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS) {
			Camera::s_DoGravity = false;
			doPlayerCollision = false;
		}
		if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS) {
			doPlayerCollision = true;
			Camera::s_DoGravity = true;
		}

		//physics
		if (Camera::s_DoGravity) {

			if (doPlayerCollision) { //testing collisions if touching ground
				if (!footCollision) { //air
					if (TA3.Counter >= 0.20f) {
						camera.DoJump = false;
						camera.Position = glm::vec3(cameraPos.x, (cameraPos.y - (10 * deltaTime)), cameraPos.z);
					}
					else {
						TA3.update();
					}

				}
				else { //ground
					camera.DoJump = true;
					TA3.reset();

					if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) { //gravity
						camera.Position = glm::vec3(cameraPos.x, (cameraPos.y - (10 * deltaTime)), cameraPos.z);
					}


				}
			}
			//crouch 
			if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
				PlayerHeightCurrent = CrouchHighDiff;
			}
			else {
				PlayerHeightCurrent = PlayerHeight;
			}

		}
		else {
			camera.DoJump = true;
			PlayerHeightCurrent = PlayerHeight;
		}

		// Check for collision
		if (doPlayerCollision) {
			if (feetpos.y <= planeY && cameraPos.y >= planeY && // Check if plane is between feetpos and cameraPos
				feetpos.x >= planeMinX && feetpos.x <= planeMaxX && // Check X bounds
				feetpos.z >= planeMinZ && feetpos.z <= planeMaxZ) { // Check Z bounds
				// Collision detected
				camera.Position = glm::vec3(cameraPos.x, PlayerHeightCurrent, cameraPos.z);
				footCollision = true;
			}
			else {
				footCollision = false;
			}
		}

		if (glfwGetKey(window, GLFW_KEY_F10) == GLFW_PRESS) {
			camera.Position = glm::vec3(0, 0, 0);
		}
		//physics

		if (glfwGetKey(window, GLFW_KEY_HOME) == GLFW_PRESS) { loadSettings(); loadEngineSettings(); }

		//FrameBuffer
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		// Clear BackBuffer
		glClearColor(skyRGBA[0], skyRGBA[1], skyRGBA[2], skyRGBA[3]), glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // Clear with colour
		glEnable(GL_DEPTH_TEST); // this line here caused me so much hell

		switch (Main::TempButton) {
		case -1: { loadShaderProgram(Main::VertNum, Main::FragNum, shaderProgram); Main::TempButton = 0; break; }
		case 1: { camera.Position = glm::vec3(0, 0, 0); Main::TempButton = 0; break; }
		case 2: { camera.Position = glm::vec3(CameraXYZ[0], CameraXYZ[1], CameraXYZ[2]); Main::TempButton = 0; break; }
		}

		// Convert variables to glm variables which hold data like a table
		glm::vec3 lightPos = glm::vec3(LightTransform1[0], LightTransform1[1], LightTransform1[2]);
		glm::mat4 lightModel = glm::mat4(1.0f); lightModel = glm::translate(lightModel, lightPos);

		// Send Variables to shader (GPU)
		shaderProgram.Activate(); // activate shaderprog to send uniforms to gpu
		UF::Bool(shaderProgram.ID, "doReflect", doReflections);
		UF::Bool(shaderProgram.ID, "doFog", doFog);
		UF::TrasformUniforms(shaderProgram.ID, ConeSI, ConeRot, lightPos);
		UF::Depth(shaderProgram.ID, DepthDistance, DepthPlane);
		UF::ColourUniforms(shaderProgram.ID, fogRGBA, skyRGBA, lightRGBA, gamma);

		//UniformH.Float3(shaderProgram.ID, "Transmodel", NULL, NULL, NULL); // testing
		LightProgram.Activate();
		UF::Float4(LightProgram.ID, "lightColor", lightRGBA[0], lightRGBA[1], lightRGBA[2], lightRGBA[3]);
		//UniformH.Float3(LightProgram.ID, "Lightmodel", lightPos.x, lightPos.y, lightPos.z);
		// Camera
		camera.Inputs(window); // send Camera.cpp window inputs and delta time
		camera.updateMatrix(cameraSettings[0], cameraSettings[1], cameraSettings[2]); // Update: fov, near and far plane

		// draw the model
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);

		if (isWireframe) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Enable wireframe mode
			glClearColor(0, 0, 0, 1), glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		}

		for (auto& modelTuple : models) {
			Model& model = std::get<0>(modelTuple);
			int cullingSetting = std::get<1>(modelTuple);
			glm::vec3 translation = std::get<2>(modelTuple);
			glm::quat rotation = std::get<3>(modelTuple);
			glm::vec3 scale = std::get<4>(modelTuple);

			// Apply culling settings
			if (cullingSetting == 1 && !isWireframe) { glEnable(GL_CULL_FACE); }
			else { glDisable(GL_CULL_FACE); }

			if (!isWireframe) {
				model.Draw(shaderProgram, camera, translation, rotation, scale);
				glDisable(GL_CULL_FACE);
				Lightmodel.Draw(LightProgram, camera, lightPos, glm::quat(0, 0, 0, 0), glm::vec3(1.0f, 1.0f, 1.0f));
			}
			else {
				SolidColour.Activate();
				UF::Depth(SolidColour.ID, 50, DepthPlane);

				model.Draw(SolidColour, camera, translation, rotation, scale);
				glDisable(GL_CULL_FACE);
				Lightmodel.Draw(SolidColour, camera, lightPos, glm::quat(0, 0, 0, 0), glm::vec3(1.0f, 1.0f, 1.0f));
			}
		}

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Restore normal rendering < wireframe

		if (Stencil) {
			glStencilFunc(GL_NOTEQUAL, 1, 0XFF);
			glStencilMask(0x00);
			glDisable(GL_DEPTH_TEST);
			outlineShaderProgram.Activate();
			UF::Float(outlineShaderProgram.ID, "outlining", stencilSize);
			UF::Float4(outlineShaderProgram.ID, "stencilColor", stencilColor[0], stencilColor[1], stencilColor[2], stencilColor[3]);
			// add stencil buffer toggle tommorow
			// draw
			for (auto& modelTuple : models) {
				Model& model = std::get<0>(modelTuple);
				glm::vec3 translation = std::get<2>(modelTuple);
				glm::quat rotation = std::get<3>(modelTuple);
				glm::vec3 scale = std::get<4>(modelTuple);
				model.Draw(outlineShaderProgram, camera, translation, rotation, scale);
			}


			glStencilMask(0xFF);
			glStencilFunc(GL_ALWAYS, 0, 0xFF);
			glEnable(GL_DEPTH_TEST);
		}

		camera.Matrix(shaderProgram, "camMatrix"); // Send Camera Matrix To Shader Prog
		camera.Matrix(LightProgram, "camMatrix"); // Send Camera Matrix To Shader Prog

		// Since the cubemap will always have a depth of 1.0, we need that equal sign so it doesn't get discarded
		glDepthFunc(GL_LEQUAL);

		skyboxShader.Activate();
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		// We make the mat4 into a mat3 and then a mat4 again in order to get rid of the last row and column
		// The last row and column affect the translation of the skybox (which we don't want to affect)
		view = glm::mat4(glm::mat3(glm::lookAt(camera.Position, camera.Position + camera.Orientation, camera.Up)));
		projection = glm::perspective(glm::radians(cameraSettings[0]), (float)width / height, cameraSettings[1], cameraSettings[2]);
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		skyboxShader.Activate();
		glUniform1i(glGetUniformLocation(skyboxShader.ID, "skybox"), 0);
		UF::Float3(skyboxShader.ID, "skyRGBA", skyRGBA[0], skyRGBA[1], skyRGBA[2]);

		// Draws the cubemap as the last object so we can save a bit of performance by discarding all fragments
		// where an object is present (a depth of 1.0f will always fail against any object's depth value)
		if (!isWireframe) {
			glBindVertexArray(skyboxVAO);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
		// Switch back to the normal depth function

		glDepthFunc(GL_LESS);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		frameBufferProgram.Activate();
		UF::Float(frameBufferProgram.ID, "time", glfwGetTime());
		UF::Float(frameBufferProgram.ID, "deltaTime", TimeUtil::s_DeltaTime);

		UF::Float(frameBufferProgram.ID, UniformInput, UniformFloat[0]);
		UF::Bool(frameBufferProgram.ID, "enableFB", enableFB);

		// draw the framebuffer
		glBindVertexArray(viewVAO);
		glDisable(GL_DEPTH_TEST); // stops culling on the rectangle the framebuffer is drawn on
		glBindTexture(GL_TEXTURE_2D, frameBufferTexture);

		// Apply post-processing and render to the second FBO
		glBindFramebuffer(GL_FRAMEBUFFER, FBO2);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		frameBufferProgram.Activate();
		glBindVertexArray(viewVAO);
		glDisable(GL_DEPTH_TEST);
		glBindTexture(GL_TEXTURE_2D, frameBufferTexture);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// Copy the contents of the second FBO to the default FBO
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		frameBufferProgram.Activate();
		glBindVertexArray(viewVAO);
		glDisable(GL_DEPTH_TEST);
		glBindTexture(GL_TEXTURE_2D, frameBufferTexture2);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		if (Panels[0]) { imGuiMAIN(window, shaderProgram, primaryMonitor, camera, frameBufferTexture, RBO, FBO, frameBufferTexture2); }


		glfwSwapBuffers(window); // Swap BackBuffer with FrontBuffer (DoubleBuffering)
		glfwPollEvents(); // Tells open gl to proccess all events such as window resizing, inputs (KBM)
	}
	// Cleanup: Delete all objects on close

	ImGui_ImplOpenGL3_Shutdown(), ImGui_ImplGlfw_Shutdown(), ImGui::DestroyContext(); // Kill ImGui

	frameBufferProgram.Delete();
	LightProgram.Delete();
	skyboxShader.Delete();
	SolidColour.Delete();
	shaderProgram.Delete(); // Delete Shader Prog
	outlineShaderProgram.Delete();
	glfwDestroyWindow(window), glfwTerminate(); // Kill opengl
	return 0;
}