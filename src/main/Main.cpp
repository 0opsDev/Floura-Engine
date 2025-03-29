#include"Model.h"
#include "Camera.h"
#include "Main.h"
#include "UF.h"
#include "Init.h"
#include "screenutils.h" 
#include <glm/gtx/string_cast.hpp>
#include "timeUtil.h" 
#include "inputUtil.h"


using json = nlohmann::json;
unsigned int FBO2, frameBufferTexture2, RBO2, viewVAO, viewVBO, FBO, frameBufferTexture, RBO;// FBO init

// Forward declaration of the function
void updateFrameBufferResolution(unsigned int& frameBufferTexture, unsigned int& RBO, unsigned int& frameBufferTexture2, unsigned int& RBO2, unsigned int width, unsigned int height);

int MSAAsamp = 16.0f;
float sharpenStrength = 3;
bool enableMSAA = true; // Change this as needed
float texelSizeMulti = 1.0;
char UniformInput[64] = {0}; // 64 is buffer size
float UniformFloat[] = {0, 0, 0};

float ViewportVerticies[] = {
	// Coords,   Texture cords
	 1.0f, -1.0f,  1.0f, 0.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	-1.0f,  1.0f,  0.0f, 1.0f,

	 1.0f,  1.0f,  1.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	-1.0f,  1.0f,  0.0f, 1.0f
};

float sensitivity = 100.0f; // mouse sensitivity (please put this into the settings json, have it in imgui too and have to ability to save to it)
bool invertMouse[2] = { false, false }; // invert mouse x and y axis

//Global Variables
	//GLfloat, Render, Camera, Light
GLfloat ConeSI[3] = { 0.111f, 0.825f , 2.0f }, ConeRot[3] = { 0.0f, -1.0f , 0.0f },
LightTransform1[3] = { 0.0f, 5.0f, 0.0f }, CameraXYZ[3] = { 0.0f, 0.0f, 0.0f }, // cameraxyz values are used for initial camera position
lightRGBA[4] = { 0.0f, 0.0f, 0.0f, 1.0f }, skyRGBA[4] = { 1.0f, 1.0f, 1.0f, 1.0f },
fogRGBA[4] = { 1.0f, 1.0f, 1.0f, 1.0f }, DepthDistance = 100.0f, DepthPlane[2] = { 0.1f, 100.0f };

//Render
struct RenderSettings { int doReflections = 1, doFog = 1; bool doVsync = false, clearColour = false, frontFaceSide = false; }; RenderSettings render;

//Shader
struct ShaderSettings { int VertNum = 0, FragNum = 2; bool Stencil = 0; float stencilSize = 0.009f, stencilColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f }, gamma = 2.2; };

ShaderSettings shaderStr;

struct ScreenSettings { //Screen
	unsigned int width = 800, height = 600;
	int windowedPosX, windowedPosY, windowedWidth, windowedHeight, widthI, heightI;
	bool isFullscreen = false;
	std::string WindowTitle = "OpenGL Window";
}; ScreenSettings screen = { 0 };

struct DeltaTime { //DeltaTime
	int frameRateI, frameRate1IHZ;
	float lastFrameTime, ftDif;
	bool aqFPS = true;
	std::string framerate;
}; DeltaTime deltaTimeStr = { 0 };
static float timeAccumulator[3] = { 0.0f, 0.0f, 0.0f }; // DeltaTime Accumulators

int TempButton = 0;
bool Panels[3] = { true, true, true }; // ImGui Panels

float cameraSettings[3] = { 60.0f, 0.1f, 1000.0f }; // Float, DeltaTime, Camera: FOV , near, far

std::string mapName = ""; // String, Maploading


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

void loadShaderProgram(int VertNum, int FragNum, Shader& shaderProgram) {
	try {
		std::pair<std::string, std::string> shaderPaths = getShaderPaths(VertNum, FragNum);
		std::string vertFile = shaderPaths.first;
		std::string fragFile = shaderPaths.second;

		std::cout << "Vert: " << vertFile << " Frag: " << fragFile << std::endl;

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
			models.emplace_back(Model((mapName + path).c_str()), isCulling, location, rotation, scale);
			std::cout << "Loaded model: " << '"' << name << '"' << " from path: " << path << " at location: " << glm::to_string(location) << std::endl;
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Error processing JSON data: " << e.what() << std::endl;
		models.emplace_back(Model("Assets/assets/fallback/model/placeholder/placeholder.gltf"), 0, glm::vec3(0.0f, 0.0f, 0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	}

	return models;
}

//Methods
// Loads Settings From Files
void loadSettings() {
	// Load Settings.json
	std::ifstream settingsFile("Settings/Settings.json");
	if (settingsFile.is_open()) {
		json settingsData;
		settingsFile >> settingsData;
		settingsFile.close();

		screen.heightI = settingsData[0]["Height"];
		screen.widthI = settingsData[0]["Width"];
		screen.width = static_cast<unsigned int>(screen.widthI);
		screen.height = static_cast<unsigned int>(screen.heightI); // cast screenArea from screenAreaI

		render.doVsync = settingsData[0]["Vsync"];
		cameraSettings[0] = settingsData[0]["FOV"];
		mapName = "Assets/Maps/" + settingsData[0]["MAP"].get<std::string>() + "/";

		sensitivity = settingsData[0]["Sensitivity"];
		invertMouse[0] = settingsData[0]["InvertX"];
		invertMouse[1] = settingsData[0]["InvertY"];
	}
	else {
		std::cerr << "Failed to open Settings/Settings.json" << std::endl;
	}

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

		cameraSettings[1] = std::stof(engineDefaultData[0]["NearPlane"].get<std::string>());
		cameraSettings[2] = std::stof(engineDefaultData[0]["FarPlane"].get<std::string>());
		screen.WindowTitle = engineDefaultData[0]["Window"];
	}
	else {
		std::cerr << "Failed to open Settings/Default/EngineDefault.json" << std::endl;
	}

	// Load imguiPanels.json
	std::ifstream imguiPanelsFile("Settings/imguiPanels.json");
	if (imguiPanelsFile.is_open()) {
		json imguiPanelsData;
		imguiPanelsFile >> imguiPanelsData;
		imguiPanelsFile.close();

		Panels[0] = imguiPanelsData[0]["imGui"];
		Panels[1] = imguiPanelsData[0]["panelMain"];
		Panels[2] = imguiPanelsData[0]["Panel Performance"];
	}
	else {
		std::cerr << "Failed to open Settings/imguiPanels.json" << std::endl;
	}
}

// Holds ImGui Variables and Windows
void imGuiMAIN(GLFWwindow* window, Shader shaderProgramT, GLFWmonitor* monitorT, ScreenUtils ScreenH, float deltaTime, unsigned int& frameBufferTexture, unsigned int& RBO, unsigned int& FBO, unsigned int& frameBufferTexture2) {
	//Tell Imgui a new frame is about to begin
	ImGui_ImplOpenGL3_NewFrame(); ImGui_ImplGlfw_NewFrame(); ImGui::NewFrame();
	//Main Panel
	if (Panels[1]) {
		ImGui::Begin("Settings"); // ImGUI window creation

		ImGui::Text("Settings (Press escape to use mouse)");
		if (ImGui::SmallButton("load")) { loadSettings(); } // load settings button
		ImGui::Checkbox("Preformance Profiler", &Panels[2]);
		// Toggle ImGui Windows
		// Rendering panel
		if (ImGui::TreeNode("Rendering")) {
			if (ImGui::TreeNode("Framerate And Resolution")) {
				ImGui::Text("Framerate Limiters");
				ImGui::Checkbox("Vsync", &render.doVsync); // Set the value of doVsync (bool)
				// Screen
				ImGui::DragInt("Width", &screen.widthI);
				ImGui::DragInt("Height", &screen.heightI); // screen slider
				ImGui::Checkbox("Enable MSAA", &enableMSAA); // Set the value of doVsync (bool)
				ImGui::DragInt("MSAA samples", &MSAAsamp);
				ImGui::DragFloat("MSAA Sharpen Strength ", &sharpenStrength);
				ImGui::DragFloat("texel Size Multiplier (Edge Size)", &texelSizeMulti);

				if (ImGui::SmallButton("Apply Changes?")) { // apply button
					screen.width = static_cast<unsigned int>(screen.widthI);
					screen.height = static_cast<unsigned int>(screen.heightI); // cast screenArea from screenAreaI
					ScreenH.SetScreenSize(window, screen.width, screen.height); // set window and viewport w&h
					ScreenH.setVSync(render.doVsync); // Set Vsync to value of doVsync (bool)
					updateFrameBufferResolution(frameBufferTexture, RBO, frameBufferTexture2, RBO2, screen.width, screen.height); // Update frame buffer resolution
				}
				if (ImGui::SmallButton("Toggle Fullscreen (WARNING WILL TOGGLE HDR OFF)"))
				{
					ScreenH.toggleFullscreen(window, monitorT, screen.isFullscreen, screen.windowedPosX, screen.windowedPosY, screen.windowedWidth, screen.windowedHeight);
				} //Toggle Fullscreen


				ImGui::TreePop();// Ends The ImGui Window
			}

			if (ImGui::TreeNode("Shaders")) {
				//Optimisation And Shaders
				ImGui::Checkbox("ClearBufferBit (BackBuffer)", &render.clearColour); // Clear Buffer
				ImGui::Checkbox("Enable Stencil Buffer", &shaderStr.Stencil);
				ImGui::DragFloat("Stencil Size", &shaderStr.stencilSize);
				ImGui::DragInt("Shader Number (Vert)", &shaderStr.VertNum);
				ImGui::DragInt("Shader Number (Frag)", &shaderStr.FragNum); // Shader Switching
				if (ImGui::SmallButton("Apply Shader?")) { shaderProgramT.Delete(); TempButton = -1; } // apply shader
				ImGui::DragFloat("Gamma", &shaderStr.gamma);
				ImGui::SliderInt("doReflections", &render.doReflections, 0, 2);
				ImGui::SliderInt("doFog", &render.doFog, 0, 1); 		//Toggles

				ImGui::Text("DepthBuffer Settings (FOG)");
				ImGui::DragFloat("Depth Distance (FOG)", &DepthDistance);
				ImGui::DragFloat2("Near and Far Depth Plane", DepthPlane);

				ImGui::InputText("Uniform Input", UniformInput, IM_ARRAYSIZE(UniformInput));
				ImGui::DragFloat("UniformFloat", UniformFloat);
				if (false & UniformInput != NULL) { // Debug
					std::cout << UniformInput << std::endl;
				}

				ImGui::TreePop();// Ends The ImGui Window
			}
			// Lighting panel

			if (ImGui::TreeNode("Lighting")) {

				if (ImGui::TreeNode("Colour")) {
					ImGui::ColorEdit4("sky RGBA", skyRGBA);
					ImGui::ColorEdit4("light RGBA", lightRGBA);
					ImGui::ColorEdit4("fog RGBA", fogRGBA);	// sky and light
					ImGui::ColorEdit4("Stencil RGBA", shaderStr.stencilColor);
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
			ImGui::TreePop();// Ends The ImGui Window
		}
		// Camera panel
		if (ImGui::TreeNode("Camera Settings")) {

			if (ImGui::TreeNode("View")) {
				ImGui::SliderFloat("FOV", &cameraSettings[0], 0.1f, 160.0f); //FOV
				ImGui::DragFloat2("Near and Far Plane", &cameraSettings[1], cameraSettings[2]); // Near and FarPlane
				ImGui::TreePop();// Ends The ImGui Window 
			}

			if (ImGui::TreeNode("Transform")) {
				if (ImGui::SmallButton("Reset Camera")) { TempButton = 1; } // reset cam pos
				ImGui::DragFloat3("Camera Transform", CameraXYZ); // set cam pos
				if (ImGui::SmallButton("Set")) { TempButton = 2; } // apply cam pos
				ImGui::TreePop();// Ends The ImGui Window 
			}

			ImGui::TreePop();// Ends The ImGui Window
		}
		ImGui::End();
	}
	// preformance profiler
	if (Panels[2]) {
		ImGui::Begin("Preformance Profiler");
		// Framerate graph
		ImGui::Checkbox("Stabe Graph (Less Smoothness)", &deltaTimeStr.aqFPS);

		static float framerateValues[60] = { 0 };
		static int frValues_offset = 0;
		framerateValues[frValues_offset] = static_cast<float>(deltaTimeStr.frameRateI);
		frValues_offset = (frValues_offset + 1) % IM_ARRAYSIZE(framerateValues);

		ImGui::Text(deltaTimeStr.framerate.c_str());
		//ftDif = current frame rate(PER SEC) + half of current frame rate so the graph has space to display(max graph height
		ImGui::PlotLines("Framerate (FPS) Graph (500SAMP)", framerateValues, (IM_ARRAYSIZE(framerateValues)), frValues_offset, nullptr, 0.0f, deltaTimeStr.ftDif, ImVec2(0, 80));

		//Frame time graph
		static float frameTimeValues[90] = { 0 }; //stores 90 snapshots of frametime

		static int ftValues_offset = 0;
		frameTimeValues[ftValues_offset] = deltaTime * 1000.0f; // Convert to milliseconds
		ftValues_offset = (ftValues_offset + 1) % IM_ARRAYSIZE(frameTimeValues);
		std::string frametimes = "Frame Times " + std::to_string(frameTimeValues[ftValues_offset] = deltaTime * 1000.0f) + " ms";

		ImGui::Text(frametimes.c_str());
		ImGui::PlotLines("Frame Times (ms) Graph (90SAMP)", frameTimeValues, IM_ARRAYSIZE(frameTimeValues), ftValues_offset, nullptr, 0.0f, 50.0f, ImVec2(0, 80));
		ImGui::End();
	}

	ImGui::Begin("ViewPort");
	const float window_width = ImGui::GetContentRegionAvail().x;
	const float window_height = ImGui::GetContentRegionAvail().y;
	updateFrameBufferResolution(frameBufferTexture, RBO, frameBufferTexture2, RBO2, window_width, window_height); // Update frame buffer resolution
	glViewport(0, 0, window_width, window_height);

	    // Bind the framebuffer texture
//    glBindTexture(GL_TEXTURE_2D, frameBufferTexture);

//	ImVec2 pos = ImGui::GetCursorScreenPos();
	ImGui::Image((ImTextureID)(uintptr_t)frameBufferTexture2, ImVec2(window_width, window_height), ImVec2(0, 1), ImVec2(1, 0));
	ImGui::End();
	ImGui::Render(); // Renders the ImGUI elements

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
// Holds DeltaTime Based Variables and Functions
void DeltaMain(GLFWwindow* window, float deltaTime) { // work on this more

	// Framerate tracking
	deltaTimeStr.frameRateI = 1.0f / deltaTime;
	timeAccumulator[0] += deltaTime;
	// 1hz
	if (timeAccumulator[0] >= 1.0f) {
		deltaTimeStr.frameRate1IHZ = 1.0f / deltaTime;
		deltaTimeStr.framerate = "FPS " + std::to_string(deltaTimeStr.frameRateI);
		glfwSetWindowTitle(window, (screen.WindowTitle + " (FPS:" + std::to_string(deltaTimeStr.frameRateI) + ")").c_str());
		timeAccumulator[0] = 0.0f;
	}
	timeAccumulator[1] += deltaTime;
	// 60hz
	if (timeAccumulator[1] >= 0.016f) {
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_5) == GLFW_PRESS) { cameraSettings[0] += 0.4f; }
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_4) == GLFW_PRESS) { cameraSettings[0] -= 0.4f; }
		if (cameraSettings[0] <= 0.00f) { cameraSettings[0] = 0.1f; }
		if (cameraSettings[0] >= 160.1f) { cameraSettings[0] = 160.0f; }
		timeAccumulator[1] = 0.0f;
	}
	timeAccumulator[2] += deltaTime;

	switch (deltaTimeStr.aqFPS) {
	case true:
		if (timeAccumulator[2] >= (1000.0f / (deltaTimeStr.frameRateI * (deltaTime * 1000.0f)))) {
			deltaTimeStr.ftDif = (deltaTimeStr.frameRateI + (deltaTimeStr.frameRateI / 2));
			timeAccumulator[2] = 0.0f;
		}
		break;
	case false:
		if (timeAccumulator[2] >= (1000.0f / (deltaTimeStr.frameRateI * (10.0f)))) {
			deltaTimeStr.ftDif = (deltaTimeStr.frameRateI + (deltaTimeStr.frameRateI / 2));
			timeAccumulator[2] = 0.0f;
		}
		break;
	}
}

void setupMainFBO(unsigned int& viewVAO, unsigned int& viewVBO, unsigned int& FBO, unsigned int& frameBufferTexture, unsigned int& RBO, unsigned int width, unsigned int height, const float* ViewportVerticies) {
	// Initialize viewport rectangle object drawn to viewport with framebuffer texture attached
	glGenVertexArrays(1, &viewVAO);
	glGenBuffers(1, &viewVBO);
	glBindVertexArray(viewVAO);
	glBindBuffer(GL_ARRAY_BUFFER, viewVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ViewportVerticies) * 6 * 4, ViewportVerticies, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	// FrameBuffer Object
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	// ColorBuffer
	glGenTextures(1, &frameBufferTexture);
	glBindTexture(GL_TEXTURE_2D, frameBufferTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameBufferTexture, 0);

	// DepthBuffer + StencilBuffer
	// RenderBuffer Object
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

	// Error checking
	auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer error: " << fboStatus << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void setupSecondFBO(unsigned int& FBO, unsigned int& frameBufferTexture, unsigned int& RBO, unsigned int width, unsigned int height) {
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	// Color buffer
	glGenTextures(1, &frameBufferTexture);
	glBindTexture(GL_TEXTURE_2D, frameBufferTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameBufferTexture, 0);

	// Depth and stencil buffer
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

	// Error checking
	auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer error: " << fboStatus << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void updateFrameBufferResolution(unsigned int& frameBufferTexture, unsigned int& RBO, unsigned int& frameBufferTexture2, unsigned int& RBO2, unsigned int width, unsigned int height) {
	// Update first frame buffer texture
	glBindTexture(GL_TEXTURE_2D, frameBufferTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Update first render buffer storage
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// Update second frame buffer texture
	glBindTexture(GL_TEXTURE_2D, frameBufferTexture2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Update second render buffer storage
	glBindRenderbuffer(GL_RENDERBUFFER, RBO2);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

//Main Function
int main()
{
	init init; init.initGLFW(); // initialize glfw
	ScreenUtils ScreenH;
	inputUtil inputH;
	// Get the video mode of the primary monitor
	// Get the primary monitor
	GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
	if (!primaryMonitor) { std::cerr << "Failed to get primary monitor" << std::endl; glfwTerminate(); return -1; }

	// Get the video mode of the primary monitor
	const GLFWvidmode* videoMode = glfwGetVideoMode(primaryMonitor);
	if (!videoMode) { std::cerr << "Failed to get video mode" << std::endl; glfwTerminate(); return -1; }

	// second fallback
	// Store the width and height in the test array
	screen.width = videoMode->width;
	screen.height = videoMode->height;

	// Now call glfwGetMonitorPos with correct arguments
	glfwGetMonitorPos(glfwGetPrimaryMonitor(), &screen.widthI, &screen.heightI);
	loadSettings();

	//    GLFWwindow* window = glfwCreateWindow(videoMode->width, videoMode->height, "Farquhar Engine OPEN GL - 1.3", primaryMonitor, NULL);
	GLFWwindow* window = glfwCreateWindow(videoMode->width, videoMode->height, screen.WindowTitle.c_str(), NULL, NULL); // create window

	// error checking
	if (window == NULL) { std::cout << "failed to create window" << std::endl; glfwTerminate(); return -1; } // "failed to create window"

	glfwMakeContextCurrent(window);	//make window current context

	gladLoadGL(); // load open gl config

	// Enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//area of open gl we want to render in
	//screen assignment after fallback
	ScreenH.SetScreenSize(window, screen.width, screen.height);  // set window and viewport w&h
	std::cout << "Primary monitor resolution: " << screen.width << "x" << screen.height << std::endl;
	// window logo creation and assignment
	init.initLogo(window);
	ScreenH.setVSync(render.doVsync); // Set Vsync to value of doVsync (bool)


	UF UniformH; // glunfiorm

	// shaderprog init
	Shader shaderProgram("Shaders/Empty.shader", "Shaders/Empty.shader"); // create a shader program and feed it Dummy shader and vertex files
	shaderProgram.Delete(); // clean the shader prog for memory management
	loadShaderProgram(shaderStr.VertNum, shaderStr.FragNum, shaderProgram);// feed the shader prog real data
	shaderProgram.Activate(); // activate new shader program for use

	Shader outlineShaderProgram("Shaders/Main/outlining.vert", "Shaders/Main/outlining.frag");
	Shader LightProgram("Shaders/Db/light.vert", "Shaders/Db/light.frag");

	Shader frameBufferProgram("Shaders/Main/framebuffer.vert", "Shaders/Main/framebuffer.frag");
	frameBufferProgram.Activate();
	UniformH.Int(frameBufferProgram.ID, "screenTexture", 0);

	// glenables
	// depth pass. render things in correct order. eg sky behind wall, dirt under water, not random order
	init.initGLenable(render.frontFaceSide);

	// INITIALIZE CAMERA
	Camera camera(screen.width, screen.height, glm::vec3(0.0f, 0.0f, 50.0f)); 	// camera ratio pos
	camera.Position = glm::vec3(CameraXYZ[0], CameraXYZ[1], CameraXYZ[2]); // camera ratio pos //INIT CAMERA POSITION
	// Model Loader
	std::vector<std::tuple<Model, int, glm::vec3, glm::quat, glm::vec3>> models = loadModelsFromJson(mapName + "ModelECSData.json"); // Load models from JSON file

	Model Lightmodel = "Assets/assets/Light/light.gltf";

	setupMainFBO(viewVAO, viewVBO, FBO, frameBufferTexture, RBO, screen.width, screen.height, ViewportVerticies);
	setupSecondFBO(FBO2, frameBufferTexture2, RBO2, screen.width, screen.height);

	init.initImGui(window); // Initialize ImGUI

	if (Panels[0]) { float deltaTime = TimeUtil::deltaTime; imGuiMAIN(window, shaderProgram, primaryMonitor, ScreenH, deltaTime, frameBufferTexture, RBO, FBO, frameBufferTexture2); } //dummy deltatime for init + imgui

	while (!glfwWindowShouldClose(window)) // GAME LOOP
	{

		inputH.updateMouse(invertMouse, sensitivity); // update mouse
		if (glfwGetKey(window, GLFW_KEY_HOME) == GLFW_PRESS) { loadSettings(); }

		// Update delta time
		TimeUtil::updateDeltaTime();
		float deltaTime = TimeUtil::deltaTime;
		DeltaMain(window, deltaTime); // Calls the DeltaMain Method that Handles variables that require delta time (FrameTime, FPS, ETC) \

		//FrameBuffer
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		// Clear BackBuffer
		if (render.clearColour) { glClear(GL_DEPTH_BUFFER_BIT); } // clear just depth buffer for lols
		else { glClearColor(skyRGBA[0], skyRGBA[1], skyRGBA[2], skyRGBA[3]), glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); } // Clear with colour
		glEnable(GL_DEPTH_TEST); // this line here caused me so much hell

		switch (TempButton) {
		case -1: { loadShaderProgram(shaderStr.VertNum, shaderStr.FragNum, shaderProgram); TempButton = 0; break; }
		case 1: { camera.Position = glm::vec3(0, 0, 0); TempButton = 0; break; }
		case 2: { camera.Position = glm::vec3(CameraXYZ[0], CameraXYZ[1], CameraXYZ[2]); TempButton = 0; break; }
		}

		// Convert variables to glm variables which hold data like a table
		glm::vec3 lightPos = glm::vec3(LightTransform1[0], LightTransform1[1], LightTransform1[2]);
		glm::mat4 lightModel = glm::mat4(1.0f); lightModel = glm::translate(lightModel, lightPos);

		// Send Variables to shader (GPU)
		shaderProgram.Activate(); // activate shaderprog to send uniforms to gpu
		UniformH.DoUniforms(shaderProgram.ID, render.doReflections, render.doFog);
		UniformH.TrasformUniforms(shaderProgram.ID, ConeSI, ConeRot, lightPos, DepthDistance, DepthPlane);
		UniformH.ColourUniforms(shaderProgram.ID, fogRGBA, skyRGBA, lightRGBA, shaderStr.gamma);


		//UniformH.Float3(shaderProgram.ID, "Transmodel", NULL, NULL, NULL); // testing
		LightProgram.Activate();
		UniformH.Float4(LightProgram.ID, "lightColor", lightRGBA[0], lightRGBA[1], lightRGBA[2], lightRGBA[3]);
		//UniformH.Float3(LightProgram.ID, "Lightmodel", lightPos.x, lightPos.y, lightPos.z);
		// Camera
		camera.Inputs(window); // send Camera.cpp window inputs and delta time
		camera.updateMatrix(cameraSettings[0], cameraSettings[1], cameraSettings[2]); // Update: fov, near and far plane

		// draw the model
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);


		if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Enable wireframe mode
			glClearColor(0, 0.3, 0.4, 1), glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		}

		for (auto& modelTuple : models) {
			Model& model = std::get<0>(modelTuple);
			int cullingSetting = std::get<1>(modelTuple);
			glm::vec3 translation = std::get<2>(modelTuple);
			glm::quat rotation = std::get<3>(modelTuple);
			glm::vec3 scale = std::get<4>(modelTuple);

			// Apply culling settings
			if (cullingSetting == 1 && glfwGetKey(window, GLFW_KEY_F1) == GLFW_RELEASE) { glEnable(GL_CULL_FACE); }
			else { glDisable(GL_CULL_FACE); }

			model.Draw(shaderProgram, camera, translation, rotation, scale); // add arg for transform to draw inside of model class
		}
		glDisable(GL_CULL_FACE);
		Lightmodel.Draw(LightProgram, camera, lightPos, glm::quat(0, 0, 0, 0), glm::vec3(1.0f, 1.0f ,1.0f));

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Restore normal rendering < wireframe

		if (shaderStr.Stencil) {
			glStencilFunc(GL_NOTEQUAL, 1, 0XFF);
			glStencilMask(0x00);
			glDisable(GL_DEPTH_TEST);
			outlineShaderProgram.Activate();
			UniformH.Float(outlineShaderProgram.ID, "outlining", shaderStr.stencilSize);
			UniformH.Float4(outlineShaderProgram.ID, "stencilColor", shaderStr.stencilColor[0], shaderStr.stencilColor[1], shaderStr.stencilColor[2], shaderStr.stencilColor[3]);
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

		//think about updating a copy of the texture here so it can be rendered on gui with effects

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// draw the framebuffer
		GLint uniformLocation = glGetUniformLocation(frameBufferProgram.ID, "enableMSAA");
		frameBufferProgram.Activate();
		UniformH.Float(frameBufferProgram.ID, "time", glfwGetTime());
		UniformH.Int(frameBufferProgram.ID, "MSAAsamp", MSAAsamp);
		UniformH.Float(frameBufferProgram.ID, "sharpenStrength", sharpenStrength);
		UniformH.Float(frameBufferProgram.ID, "texelSizeMulti", texelSizeMulti);

		UniformH.Float(frameBufferProgram.ID,UniformInput, UniformFloat[0]);

		UniformH.Int(frameBufferProgram.ID, "frameCount", 4);
		glUniform1i(uniformLocation, enableMSAA ? 1 : 0);

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

		if (Panels[0]) { imGuiMAIN(window, shaderProgram, primaryMonitor, ScreenH, deltaTime, frameBufferTexture, RBO, FBO, frameBufferTexture2); }


		glfwSwapBuffers(window); // Swap BackBuffer with FrontBuffer (DoubleBuffering)
		glfwPollEvents(); // Tells open gl to proccess all events such as window resizing, inputs (KBM)
	}
	// Cleanup: Delete all objects on close

	ImGui_ImplOpenGL3_Shutdown(), ImGui_ImplGlfw_Shutdown(), ImGui::DestroyContext(); // Kill ImGui
	frameBufferProgram.Delete();
	shaderProgram.Delete(); // Delete Shader Prog
	outlineShaderProgram.Delete();
	glfwDestroyWindow(window), glfwTerminate(); // Kill opengl
	return 0;
}	