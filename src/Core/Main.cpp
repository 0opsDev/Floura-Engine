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
#include "Render/Shader/Cubemap.h"
#define STB_PERLIN_IMPLEMENTATION
#include <stb/stb_perlin.h>
#include "Render/Shader/Framebuffer.h"
#include "Scripting/ScriptRunner.h"
#include "imgui/imgui_impl_opengl3.h"
#include "render/Shader/SkyBox.h"
#include "File/File.h"
#include <UI/ImGui/ImGuiWindow.h>
#include "Sound/SoundProgram.h"
#include <Sound/SoundRunner.h>

bool anyCollision = false; // Track collision status
int Main::VertNum = 0, Main::FragNum = 0;
bool Main::ApplyShader = true;
bool Main::sleepState = true;
std::string DefaultSkyboxPath;

// Global Variables
GLfloat ConeSI[3] = { 0.111f, 0.825f, 2.0f };
GLfloat ConeRot[3] = { 0.0f, -1.0f, 0.0f };
GLfloat LightTransform1[] = { 0.0f, 5.0f, 0.0f };
GLfloat CameraXYZ[3] = { 0.0f, 15.0f, 0.0f }; // Initial camera position
GLfloat lightRGBA[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat skyRGBA[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat fogRGBA[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat DepthDistance = 100.0f;
GLfloat DepthPlane[2] = { 0.1f, 100.0f };

// Render settings
bool doReflections = true;
bool doFog = true;
float gamma = 2.2f;
float volume = 1;

unsigned int width = 800, height = 600;

float Main::cameraSettings[3] = { 60.0f, 0.1f, 1000.0f }; // FOV, near, far

bool doPlayerCollision = true;
float PlayerHeight = 1.8f;
float CrouchHighDiff = 0.9f;
float PlayerHeightCurrent;

float window_width;
float window_height;

TimeAccumulator TA1; TimeAccumulator TA3; TimeAccumulator TA2; TimeAccumulator TA4;
Shader shaderProgram("skip", ""); // create a shader program and feed it Dummy shader and vertex files
Camera camera(width, height, glm::vec3(0.0f, 0.0f, 50.0f)); 	// camera ratio pos

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

// Holds ImGui Variables and Windows
void imGuiMAIN(GLFWwindow* window, Shader& shaderProgramT,
	GLFWmonitor* monitorT, Camera& camera) {
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

	//Main Panel
	ImGui::Begin("Panels"); // ImGUI window creation
	ImGui::Text("Settings (Press escape to use mouse)");
	if (ImGui::SmallButton("load")) { Main::loadSettings(); Main::loadEngineSettings(); } // load settings button
	if (ImGui::SmallButton("save (just settings)")) { Main::saveSettings(); } // save settings button
	ImGui::SliderFloat("volume", &volume, 0, 1);
	ImGui::Checkbox("Rendering", &ImGuiCamera::imGuiPanels[1]);
	ImGui::Checkbox("Camera Settings", &ImGuiCamera::imGuiPanels[2]);
	ImGui::Checkbox("ViewPort", &ImGuiCamera::imGuiPanels[3]);
	//Panels
	// Toggle ImGui Windows
	// Rendering panel
	if (ImGuiCamera::imGuiPanels[1]) {

		ImGui::Begin("Rendering"); // ImGUI window creation

		if (ImGui::TreeNode("System Infomation")) {
			const GLubyte* version = glGetString(GL_VERSION);
			const GLubyte* renderer = glGetString(GL_RENDERER);

			ImGui::Text("OpenGL Version: %s", version); // Display OpenGL version
			ImGui::Text("Renderer: %s", renderer);  // Display GPU renderer

			ImGui::Text( (std::string("ViewportSize: ") + std::to_string(static_cast<int>(Framebuffer::ViewPortWidth * ImGuiCamera::resolutionScale)) + "*" + std::to_string(static_cast<int>(Framebuffer::ViewPortHeight * ImGuiCamera::resolutionScale)) ).c_str() );

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

		ImGui::Checkbox("isWireframe", &ImGuiCamera::isWireframe);

		ImGui::Dummy(ImVec2(0.0f, 5.0f)); // Adds 5 pixels of vertical space
		if (ImGui::TreeNode("Framerate And Resolution")) {
			ImGui::Text("Framerate Limiters");
			ImGui::Checkbox("Vsync", &ScreenUtils::doVsync); // Set the value of doVsync (bool)
			// Screen
			ImGui::DragInt("Width", &SettingsUtils::tempWidth);
			ImGui::DragInt("Height", &SettingsUtils::tempHeight); // screen slider

			ImGui::SliderFloat("Resolution Scale", &ImGuiCamera::resolutionScale, 0.001, 1);
			//enableLinearScaling
			ImGui::Checkbox("Enable Linear Scaling", &ImGuiCamera::enableLinearScaling); // Set the value of enableLinearScaling (bool)
			ImGui::Checkbox("Enable FB shader", &ImGuiCamera::enableFB); // Set the value of enableFB (bool)

			if (ImGui::SmallButton("Apply Changes?")) { // apply button
				glViewport(0, 0, SettingsUtils::tempWidth * ImGuiCamera::resolutionScale, SettingsUtils::tempHeight * ImGuiCamera::resolutionScale); // real internal res
				glfwSetWindowSize(window, SettingsUtils::tempWidth, SettingsUtils::tempHeight);
				ScreenUtils::setVSync(ScreenUtils::doVsync); // Set Vsync to value of doVsync (bool)
				Framebuffer::updateFrameBufferResolution(SettingsUtils::tempWidth, SettingsUtils::tempHeight); // Update frame buffer resolution
			}
			if (ImGui::SmallButton("Toggle Fullscreen (WARNING WILL TOGGLE HDR OFF)"))
			{
				ScreenUtils::toggleFullscreen(window, monitorT, width, height); //needs to be fixed
			} //Toggle Fullscreen


			ImGui::TreePop();// Ends The ImGui Window
		}

		if (ImGui::TreeNode("Shaders")) {
			//Optimisation And Shaders
			ImGui::DragInt("Shader Number (Vert)", &Main::VertNum);
			ImGui::DragInt("Shader Number (Frag)", &Main::FragNum); // Shader Switching
			if (ImGui::SmallButton("Apply Shader?")) { Main::ApplyShader = true; } // apply shader
			ImGui::DragFloat("Gamma", &gamma);
			ImGui::Checkbox("doReflections", &doReflections);
			ImGui::Checkbox("doFog", &doFog); 		//Toggles

			ImGui::Text("DepthBuffer Settings (FOG)");
			ImGui::DragFloat("Depth Distance (FOG)", &DepthDistance);
			ImGui::DragFloat2("Near and Far Depth Plane", DepthPlane);

			ImGui::InputText("Uniform Input", ImGuiCamera::UniformInput, IM_ARRAYSIZE(ImGuiCamera::UniformInput));
			ImGui::DragFloat("UniformFloat", ImGuiCamera::UniformFloat);
			if (false & ImGuiCamera::UniformInput != NULL) { // Debug
				if (init::LogALL || init::LogSystems) std::cout << ImGuiCamera::UniformInput << std::endl;
			}
			ImGui::TreePop();// Ends The ImGui Window
		}
		// Lighting panel

		if (ImGui::TreeNode("Lighting")) {

			if (ImGui::TreeNode("Colour")) {
				ImGui::ColorEdit4("sky RGBA", skyRGBA);
				ImGui::ColorEdit4("light RGBA", lightRGBA);
				ImGui::ColorEdit4("fog RGBA", fogRGBA);	// sky and light
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
	if (ImGuiCamera::imGuiPanels[2]) {
		ImGui::Begin("Camera Settings"); // ImGUI window creation
		//std::to_string(footCollision)
		ImGui::Text( ("Camera Position: x: " + std::to_string(Camera::s_PositionMatrix.x) + "y: " + std::to_string(Camera::s_PositionMatrix.y) + "z: " + std::to_string(Camera::s_PositionMatrix.z)).c_str());
		glm::quat cameraQuat = camera.Orientation; // Ensure Orientation is a quaternion
		glm::vec3 eulerAngles = glm::degrees(glm::eulerAngles(cameraQuat));
		ImGui::Text(("Camera Rotation: Yaw: " + std::to_string(eulerAngles.x) + " Pitch: " + std::to_string(eulerAngles.y) + " Roll: " + std::to_string(eulerAngles.z)).c_str());
		ImGui::Spacing();
		if (ImGui::TreeNode("Controls")) {
			ImGui::Text("Transform");
			if (ImGui::SmallButton("Reset Camera")) {camera.s_PositionMatrix = glm::vec3(0, 0, 0);
			} // reset cam pos
			ImGui::DragFloat3("Camera Transform", CameraXYZ); // set cam pos
			if (ImGui::SmallButton("Set")) { camera.s_PositionMatrix = glm::vec3(CameraXYZ[0], CameraXYZ[1], CameraXYZ[2]); } // apply cam pos
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
			ImGui::Text(("Foot Collision: " + std::to_string(anyCollision) ).c_str() );
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
		if (ScreenUtils::isResizing == true || ImGuiCamera::resolutionScale != ImGuiCamera::prevResolutionScale || ImGuiCamera::enableLinearScaling != ImGuiCamera::prevEnableLinearScaling) {
			//std::cout << "Resolution scale changed!" << std::endl;
			Framebuffer::updateFrameBufferResolution(window_width, window_height); // Update frame buffer resolution
			glViewport(0, 0, (window_width* ImGuiCamera::resolutionScale), (window_height* ImGuiCamera::resolutionScale));

			camera.SetViewportSize(window_width, window_height);
			//std::cout << window_width << " " << camera.width << std::endl;
			//std::cout << window_height << " " << camera.height << std::endl;
		}
		ImGuiCamera::prevResolutionScale = ImGuiCamera::resolutionScale; // Update the previous scale
		ImGuiCamera::prevEnableLinearScaling = ImGuiCamera::ImGuiCamera::enableLinearScaling;

		ImGui::End();
	}
	ImGui::End();
	ImGui::Render(); // Renders the ImGUI elements
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

//Main Function
int main() // global variables do not work with threads
{
	Main::sleepState = true;
	auto startInitTime = std::chrono::high_resolution_clock::now();
	init::initLog();// init logs (should always be before priniting anything)
	ScriptRunner::init();
	init::initGLFW(); // initialize glfw
	Main::loadSettings();
	Main::loadEngineSettings();
	SoundRunner::init();

	//Clair de Lune (Studio Version) 4.wav
	SoundProgram Wind; Wind.CreateSound("Assets/Sounds/Cold wind sound effect 4.wav");
	SoundProgram Soundtrack; Soundtrack.CreateSound("Assets/Sounds/Clair de Lune.wav");
	SoundProgram FootSound; FootSound.CreateSound("Assets/Sounds/Footsteps.wav");
	SoundProgram land; land.CreateSound("Assets/Sounds/land.wav");

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

	// Enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//area of open gl we want to render in
	//screen assignment after fallback
	ScreenUtils::SetScreenSize(window, width, height);  // set window and viewport w&h
	if (init::LogALL || init::LogSystems) std::cout << "Primary monitor resolution: " << width << "x" << height << std::endl;
	// window logo creation and assignment
	init::initLogo(window);
	ScreenUtils::setVSync(ScreenUtils::doVsync); // Set Vsync to value of doVsync (bool)
	// shaderprog init

	FileClass::loadShaderProgram(Main::VertNum, Main::FragNum, shaderProgram);// feed the shader prog real data
	shaderProgram.Activate(); // activate new shader program for use

	Shader LightProgram("Shaders/Lighting/light.vert", "Shaders/Lighting/light.frag");
	Shader SolidColour("Shaders/Lighting/Default.vert", "Shaders/Db/solidColour.frag");

	// move to framebuffer class
	Shader frameBufferProgram("Shaders/PostProcess/framebuffer.vert", "Shaders/PostProcess/framebuffer.frag");
	frameBufferProgram.Activate();
	UF::Int(frameBufferProgram.ID, "screenTexture", 0);

	// glenables
	// depth pass. render things in correct order. eg sky behind wall, dirt under water, not random order
	init::initGLenable(false); //bool for direction of polys

	// INITIALIZE CAMERA
	camera.Position = glm::vec3(CameraXYZ[0], CameraXYZ[1], CameraXYZ[2]); // camera ratio pos //INIT CAMERA POSITION

	Skybox::init(DefaultSkyboxPath);
	
	// put in one function
	Framebuffer::setupMainFBO(width, height);
	Framebuffer::setupSecondFBO(width, height);

	init::initImGui(window); // Initialize ImGUI

	// Model Loader
	std::vector<std::tuple<Model, int, glm::vec3, glm::quat, glm::vec3>> models = FileClass::loadModelsFromJson(SettingsUtils::mapName + "ModelECSData.json"); // Load models from JSON file 
	Model Lightmodel = "Assets/assets/Light/light.gltf";
	//Model Ground = "Assets/assets/Ground/Ground.gltf";

	Main::LoadPlayerConfig();

	auto stopInitTime = std::chrono::high_resolution_clock::now();
	auto initDuration = std::chrono::duration_cast<std::chrono::microseconds>(stopInitTime - startInitTime);
	if (init::LogALL || init::LogSystems) std::cout << "init Duration: " << initDuration.count() / 1000000.0 << std::endl;
	//imGuiPanels[0]
	while (!glfwWindowShouldClose(window)) // GAME LOOP
	{
		TimeUtil::updateDeltaTime(); float deltaTime = TimeUtil::s_DeltaTime; // Update delta time
		if (!Soundtrack.isPlay) {Soundtrack.PlaySound(volume);}
		Soundtrack.SetVolume(volume);
		Soundtrack.updateCameraPosition();
		Soundtrack.SetSoundPosition(camera.s_PositionMatrix.x, camera.s_PositionMatrix.y, camera.s_PositionMatrix.z);
		if (!Wind.isPlay) { Wind.PlaySound(0.7);}
		Wind.updateCameraPosition();
		Wind.SetSoundPosition(camera.s_PositionMatrix.x, camera.s_PositionMatrix.y, camera.s_PositionMatrix.z);
		TA1.update();
		// Update FPS and window title every second  
		if (TA1.Counter >= 1/1.0f) {
			glfwSetWindowTitle(window, (SettingsUtils::s_WindowTitle + " (FPS: " + std::to_string(static_cast<int>(TimeUtil::s_frameRate1hz) ) ).c_str());
			//std::cout << "update" << std::endl;
			TA1.reset();
		}
		InputUtil::UpdateCurrentKey(window);
		//TA2
		TA2.update();
		// Update FPS and window title every second  
		if (TA2.Counter >= 1 / 10.0f) {
			if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS) { ImGuiCamera::imGuiPanels[0] = !ImGuiCamera::imGuiPanels[0]; }
			//std::cout << "update" << std::endl;
			TA2.reset();
		}
		if (glfwGetKey(window, GLFW_KEY_BACKSLASH) == GLFW_PRESS) { ScriptRunner::clearScripts(); ScriptRunner::init(); };
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_5) == GLFW_PRESS) Main::cameraSettings[0] = std::min(Main::cameraSettings[0] + (50.0f * TimeUtil::s_DeltaTime), 160.0f);
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_4) == GLFW_PRESS) Main::cameraSettings[0] = std::max(Main::cameraSettings[0] - (50.0f * TimeUtil::s_DeltaTime), 0.1f);
		ScriptRunner::update(); 

		glm::vec3 cameraPos = Camera::s_PositionMatrix;
		glm::vec3 feetpos = glm::vec3(Camera::s_PositionMatrix.x, (Camera::s_PositionMatrix.y - PlayerHeightCurrent), Camera::s_PositionMatrix.z);

		FootSound.SetSoundPosition(feetpos.x, feetpos.y, feetpos.z);
		FootSound.updateCameraPosition();

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
			}
			//crouch 
			if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {PlayerHeightCurrent = CrouchHighDiff;}
			else {PlayerHeightCurrent = PlayerHeight;}
		}

		if (glfwGetKey(window, GLFW_KEY_HOME) == GLFW_PRESS) { Main::loadSettings(); Main::loadEngineSettings(); }

		//FrameBuffer
		glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::FBO);
		// Clear BackBuffer
		glClearColor(skyRGBA[0], skyRGBA[1], skyRGBA[2], skyRGBA[3]), glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // Clear with colour
		glEnable(GL_DEPTH_TEST); // this line here caused me so much hell

		if (Main::ApplyShader) {
		FileClass::loadShaderProgram(Main::VertNum, Main::FragNum, shaderProgram); 
		Main::ApplyShader = false;
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
		//std::cout << "After updateMatrix: " << camera.width << " x " << camera.height << std::endl;
		if (ImGuiCamera::isWireframe) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Enable wireframe mode
			glClearColor(0, 0, 0, 1), glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		}
		anyCollision = false;
		for (auto& modelTuple : models) {
			Model& model = std::get<0>(modelTuple);
			glm::vec3 translation = std::get<2>(modelTuple);
			glm::quat rotation = std::get<3>(modelTuple);
			glm::vec3 scale = std::get<4>(modelTuple);

				if (model.checkCollide(feetpos, translation, rotation, scale, 5)) {
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
						feetpos.y = detectedSurfaceY + 0.05f; // Ensure player stays above the surface

						// Scale push-back stronger for steeper slopes
						float pushStrength = glm::mix(0.05f, 0.5f, slopeSteepness / 90.0f);
						feetpos.x -= normal.x * pushStrength;
						feetpos.z -= normal.z * pushStrength;
					}
					TA3.reset();
					break; // Exit loop once collision is detected
				}

				// Apply Gravity to feetpos Only If No Collision Occurred
				if (!anyCollision && Camera::s_DoGravity) {
					feetpos.y -= (2 + TA3.Counter) * deltaTime; // Falling behavior
					if (TA3.Counter >= (1 / 0.5)) {if (FootSound.isPlay) FootSound.StopSound();camera.DoJump = false;}
					else {camera.DoJump = true;}

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
		}
		if (!Camera::s_DoGravity) { camera.DoJump = true; };

		// Camera Always Stays Above Feet Position**
		cameraPos.y = feetpos.y + PlayerHeightCurrent;
		camera.Position = cameraPos;
		Camera::s_PositionMatrix = glm::vec3(feetpos.x, feetpos.y + PlayerHeightCurrent, feetpos.z);

		for (auto& modelTuple : models) {
			Model& model = std::get<0>(modelTuple);
			int cullingSetting = std::get<1>(modelTuple);
			glm::vec3 translation = std::get<2>(modelTuple);
			glm::quat rotation = std::get<3>(modelTuple);
			glm::vec3 scale = std::get<4>(modelTuple);

			// Apply culling settings
			if (cullingSetting == 1 && !ImGuiCamera::isWireframe) { glEnable(GL_CULL_FACE); }
			else { glDisable(GL_CULL_FACE); }

			if (!ImGuiCamera::isWireframe) {
				model.Draw(shaderProgram, camera, translation, rotation, scale);
				glDisable(GL_CULL_FACE);
				Lightmodel.Draw(LightProgram, camera, lightPos, glm::quat(0, 0, 0, 0), glm::vec3(0.3f, 0.3f, 0.3f));
			}
			else {
				SolidColour.Activate();
				UF::Depth(SolidColour.ID, 50, DepthPlane);

				model.Draw(SolidColour, camera, translation, rotation, scale);
				glDisable(GL_CULL_FACE);
				Lightmodel.Draw(SolidColour, camera, lightPos, glm::quat(0, 0, 0, 0), glm::vec3(0.3f, 0.3f, 0.3f));
			}
		}

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Restore normal rendering < wireframe
		//Ground.Draw(shaderProgram, camera, feetpos, glm::quat(0, 0, 0, 0), glm::vec3(0.3f, 0.3f, 0.3f));
		// Camera
		camera.Inputs(window); // send Camera.cpp window inputs and delta time
		//std::cout << "Before updateMatrix: " << camera.width << " x " << camera.height << std::endl;
		camera.updateMatrix(Main::cameraSettings[0], Main::cameraSettings[1], Main::cameraSettings[2]); // Update: fov, near and far plane
		camera.Matrix(shaderProgram, "camMatrix"); // Send Camera Matrix To Shader Prog
		camera.Matrix(LightProgram, "camMatrix"); // Send Camera Matrix To Shader Prog

		if (!ImGuiCamera::isWireframe) {
			Skybox::draw(camera, skyRGBA, camera.width, camera.height); // cleanup later, put camera width and height inside skybox class since, they're already global
		}
		// Framebuffer logic
		Framebuffer::FBODraw(frameBufferProgram, ImGuiCamera::imGuiPanels[0], window_width, window_height, window, camera);
		if (ImGuiCamera::imGuiPanels[0]) { imGuiMAIN(window, shaderProgram, primaryMonitor, camera); }

		glfwSwapBuffers(window); // Swap BackBuffer with FrontBuffer (DoubleBuffering)
		glfwPollEvents(); // Tells open gl to proccess all events such as window resizing, inputs (KBM)
	}
	// Cleanup: Delete all objects on close
	Main::sleepState = false;

	ImGui_ImplOpenGL3_Shutdown(), ImGui_ImplGlfw_Shutdown(), ImGui::DestroyContext(); // Kill ImGui

	//FootSound.DeleteSound();
	frameBufferProgram.Delete();
	LightProgram.Delete();
	Skybox::Delete();
	land.DeleteSound();
	Soundtrack.DeleteSound();
	land.DeleteSound();
	SoundRunner::Delete();
	SolidColour.Delete();
	shaderProgram.Delete(); // Delete Shader Prog
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

		Main::cameraSettings[1] = std::stof(engineDefaultData[0]["NearPlane"].get<std::string>());
		Main::cameraSettings[2] = std::stof(engineDefaultData[0]["FarPlane"].get<std::string>());

		DepthDistance = engineDefaultData[0]["DepthDistance"];
		DepthPlane[0] = engineDefaultData[0]["DepthPlane"][0];
		DepthPlane[1] = engineDefaultData[0]["DepthPlane"][1];
		SettingsUtils::s_WindowTitle = engineDefaultData[0]["Window"];
		DefaultSkyboxPath = engineDefaultData[0]["DefaultSkyboxPath"];
	}
	else {
		std::cerr << "Failed to open Settings/Default/EngineDefault.json" << std::endl;
	}
}