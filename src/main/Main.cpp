#include"Model.h"
#include "Camera.h"
#include"imgui.h"
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <vector>
#include"imgui_impl_glfw.h"
#include"imgui_impl_opengl3.h"
#include "Main.h"

//Global Variables
//Render
struct RenderSettings { int doReflections = 1, doFog = 1; bool doVsync = false, clearColour = false; }; RenderSettings render;

//Shader
struct ShaderSettings { int VertNum = 0, FragNum = 2; };
//GLfloat, Render, Camera, Light
GLfloat ConeSI[3] = { 0.05f, 0.95f , 1.0f }, ConeRot[3] = { 0.0f, -4.0f , 0.0f },
LightTransform1[3] = { 0.0f, 25.0f, 0.0f }, CameraXYZ[3] = { 0.0f, 5.0f, 0.0f },
lightRGBA[4] = { 0.0f, 0.0f, 0.0f, 1.0f }, skyRGBA[4] = { 1.0f, 1.0f, 1.0f, 1.0f },
fogRGBA[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
ShaderSettings shaderStr;

struct ScreenSettings { //Screen
	unsigned int width = 800, height = 600;
	int windowedPosX, windowedPosY, windowedWidth, windowedHeight, widthI, heightI;
	bool isFullscreen = false;
}; ScreenSettings screen = {0};

struct DeltaTime { //DeltaTime
	int frameRateI, frameRate1IHZ;
	float lastFrameTime, deltaTime, ftDif;
	bool aqFPS = true;
	std::string framerate;
}; DeltaTime deltaTimeStr = {0};
static float timeAccumulator[3] = { 0.0f, 0.0f, 0.0f }; // DeltaTime Accumulators

int TempButton = 0;
bool Panels[1] = { true }, checkboxVar[1] = { false }; // bool

static char TB[128] = "Input";

float cameraSettings[3] = { 60.0f, 0.1f, 1000.0f }; // Float, DeltaTime, Camera: FOV , near, far

std::string mapName = ""; // String, Maploading


// Function to read lines from a file into a vector of strings
std::vector<std::string> readLinesFromFile(const std::string& filePath) {
	std::vector<std::string> lines;
	std::ifstream file(filePath);
	if (file.is_open()) {
		std::string line;
		while (std::getline(file, line)) {
			lines.push_back(line);
		}
		file.close();
	}
	else {
		std::cerr << "Failed to open file: " << filePath << std::endl;
	}
	return lines;
}
// Function to read a specific line from a file
std::string readLineFromFile(const std::string& filePath, int lineNumber) {
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
void loadShaderProgram(int VertNum, int FragNum, Shader& shaderProgram) {
	try {
		std::string vertFile = readLineFromFile("Shaders/VertList.cfg", VertNum);
		std::string fragFile = readLineFromFile("Shaders/FragList.cfg", FragNum);
		std::cout << "Vert: " << vertFile << "Frag: " << fragFile << std::endl;

		shaderProgram = Shader(vertFile.c_str(), fragFile.c_str());
	}
	catch (const std::exception& e) {
		std::cerr << "Error loading shader program: " << e.what() << std::endl;
	}
}

// Function to load models from files
std::vector<Model> loadModels(const std::string& namesFilePath, const std::string& pathsFilePath) {
	std::vector<std::string> modelNames = readLinesFromFile(namesFilePath);
	std::vector<std::string> modelPaths = readLinesFromFile(pathsFilePath);

	if (modelNames.size() != modelPaths.size()) {
		throw std::runtime_error("Model names and paths count mismatch");
	}

	std::vector<Model> models;
	for (size_t i = 0; i < modelNames.size(); ++i) {
		models.emplace_back((mapName + modelPaths[i]).c_str());
		std::cout << "Loaded model: " << '"' << modelNames[i] << '"' << " from path: " << modelPaths[i] << std::endl;
	}
	return models;
}
//Methods
// Toggles Vsync
void setVSync(bool enabled) {
	glfwSwapInterval(enabled ? 1 : 0); //Toggles Vsync
}
// Loads Settings From Files
void loadSettings() {
	//READ - settings
	std::ifstream TestFile2("Settings/Settings.ini");
	if (TestFile2.is_open())
	{
		std::string lineT;
		int lineNumber = 0;

		while (std::getline(TestFile2, lineT)) {
			lineNumber++;
			std::istringstream iss(lineT); GLfloat value;

			switch (lineNumber) {
			case 3:
				if (iss >> value) screen.width = static_cast<unsigned int>(value);
				break;
			case 4:
				if (iss >> value) screen.height = static_cast<unsigned int>(value);
				break;
			case 6:
				render.doVsync = (lineT == "VsyncT");
				std::cout << "Vsync: " << render.doVsync << std::endl;
				break;
			case 8:
				if (iss >> value) cameraSettings[0] = value;
				std::cout << "Camera FOV: " << cameraSettings[0] << std::endl;
				break;
			case 10:
				mapName = "Assets/Maps/" + lineT + "/";
				std::cout << "Map: " << mapName << std::endl;
				break;
			default:
				break;
			}
			screen.heightI = screen.height; screen.widthI = screen.width;
		}
		TestFile2.close();
		
		std::ifstream TestFile3(mapName + "Engine.ini");
		if (TestFile3.is_open())
		{
			lineNumber = 0;

			while (std::getline(TestFile3, lineT)) {
				lineNumber++;
				std::istringstream iss(lineT); GLfloat value; // variable init

				switch (lineNumber) {
				case 8:
					if (iss >> value) skyRGBA[0] = value;
					break;
				case 9:
					if (iss >> value) skyRGBA[1] = value;
					break;
				case 10:
					if (iss >> value) skyRGBA[2] = value;
					break;
				case 12:
					if (iss >> value) lightRGBA[0] = value;
					break;
				case 13:
					if (iss >> value) lightRGBA[1] = value;
					break;
				case 14:
					if (iss >> value) lightRGBA[2] = value;
					break;
				case 16:
					if (iss >> value) fogRGBA[0] = value;
					break;
				case 17:
					if (iss >> value) fogRGBA[1] = value;
					break;
				case 18:
					if (iss >> value) fogRGBA[2] = value;
					break;
				case 5:
					if (iss >> value) cameraSettings[1] = value;
					std::cout << "Camera Near Plane: " << cameraSettings[1] << std::endl;
					break;
				case 6:
					if (iss >> value) cameraSettings[2] = value;
					std::cout << "Camera Far Plane: " << cameraSettings[2] << std::endl;
					break;
				case 3:
					break;
				default:
					break;
				}
			}
			TestFile3.close();
		}
	}
}
// Initialize GLFW
void initializeGLFW() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3), glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // Window Minimum and Maximum version
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //OpenGl Profile
	glfwWindowHint(GLFW_RESIZABLE, 1); // Start Resizable
	glfwWindowHint(GLFW_MAXIMIZED, 1); // Start Maximized
	glfwWindowHint(GLFW_DEPTH_BITS, 16); // DepthBuffer Bit
}
// Initialize ImGui
void initializeImGui(GLFWwindow* window) {
	IMGUI_CHECKVERSION(), ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io, ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true), ImGui_ImplOpenGL3_Init("#version 330");
}
// Toggle Fullscreen
void toggleFullscreen(GLFWwindow* window, GLFWmonitor* monitor) {
	
	screen.isFullscreen = !screen.isFullscreen;
	if (screen.isFullscreen) {

		// Save windowed mode dimensions and position
		glfwGetWindowPos(window, &screen.windowedPosX, &screen.windowedPosY);
		glfwGetWindowSize(window, &screen.windowedWidth, &screen.windowedHeight);

		const GLFWvidmode* mode = glfwGetVideoMode(monitor); // Get the video mode of the monitor
		
		glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate); // Switch to fullscreen
	}
	else { glfwSetWindowMonitor(window, NULL, screen.windowedPosX, screen.windowedPosY, screen.windowedWidth, screen.windowedHeight, 0); } // Switch to windowed mode
}
// Holds ImGui Variables and Windows
void imGuiMAIN(GLFWwindow* window, Shader shaderProgramT, GLFWmonitor* monitorT) {
	//Tell Imgui a new frame is about to begin
	ImGui_ImplOpenGL3_NewFrame(), ImGui_ImplGlfw_NewFrame(), ImGui::NewFrame();

	ImGui::Begin("Settings"); // ImGUI window creation
	ImGui::Text("Settings (Press escape to use mouse)");
	ImGui::InputText("TB TEST", TB, IM_ARRAYSIZE(TB));
	if (ImGui::SmallButton("load")) { loadSettings(); } // load settings button

	// Toggle ImGui Windows
	ImGui::Checkbox("Preformance Profiler", &Panels[0]);
	// preformance profiler
	if (Panels[0]) {
		ImGui::Begin("Preformance Profiler");
		// Framerate graph
		ImGui::Checkbox("Stabe Graph (Less Smoothness)", &deltaTimeStr.aqFPS);

		static float framerateValues[900] = { 0 };
		static int frValues_offset = 0;
		framerateValues[frValues_offset] = deltaTimeStr.frameRateI;
		frValues_offset = (frValues_offset + 1) % IM_ARRAYSIZE(framerateValues);

		ImGui::Text(deltaTimeStr.framerate.c_str());
		//ftDif = current frame rate(PER SEC) + half of current frame rate so the graph has space to display(max graph height
		ImGui::PlotLines("Framerate (FPS) Graph (500SAMP)", framerateValues, (IM_ARRAYSIZE(framerateValues)), frValues_offset, nullptr, 0.0f, deltaTimeStr.ftDif, ImVec2(0, 80));


		//Frame time graph
		static float frameTimeValues[90] = { 0 }; //stores 90 snapshots of frametime

		static int ftValues_offset = 0;
		frameTimeValues[ftValues_offset] = deltaTimeStr.deltaTime * 1000.0f; // Convert to milliseconds
		ftValues_offset = (ftValues_offset + 1) % IM_ARRAYSIZE(frameTimeValues);
		std::string frametimes = "Frame Times " + std::to_string(frameTimeValues[ftValues_offset] = deltaTimeStr.deltaTime * 1000.0f) + " ms";

		ImGui::Text(frametimes.c_str());
		ImGui::PlotLines("Frame Times (ms) Graph (90SAMP)", frameTimeValues, IM_ARRAYSIZE(frameTimeValues), ftValues_offset, nullptr, 0.0f, 50.0f, ImVec2(0, 80));
		ImGui::End();
		ImGui::TreePop();// Ends The ImGui Window
	}
	// Rendering panel
	if (ImGui::TreeNode("Rendering")) {
		if (ImGui::TreeNode("Framerate And Resolution")) {
			ImGui::Text("Framerate Limiters");
			ImGui::Checkbox("Vsync", &render.doVsync); // Set the value of doVsync (bool)

			// Screen
			ImGui::DragInt("Width", &screen.widthI), ImGui::DragInt("Height", &screen.heightI); // screen slider

			if (ImGui::SmallButton("Apply Changes?")) { // apply button
				screen.width = static_cast<unsigned int>(screen.widthI), screen.height = static_cast<unsigned int>(screen.heightI); // cast screenArea from screenAreaI
				glViewport(0, 0, screen.width, screen.height); // Set Viewport to "screen.width", "screen.height"
				glfwSetWindowSize(window, screen.width, screen.height); // Set Window Size to "screen.width", "screen.height" on window "window"
				setVSync(render.doVsync);  // Set Vsync to value of doVsync (bool)
				ImGui::TreePop();}// Ends The ImGui Window
			if (ImGui::SmallButton("Toggle Fullscreen (WARNING WILL TOGGLE HDR OFF)")) { toggleFullscreen(window, monitorT); } //Toggle Fullscreen
		}

		if (ImGui::TreeNode("Shaders")) {
			//Optimisation And Shaders
			if (ImGui::SmallButton("Enable Culling")) { glEnable(GL_CULL_FACE); } if (ImGui::SmallButton("Disable Culling")) { glDisable(GL_CULL_FACE); } //culling
			ImGui::Checkbox("ClearColourBufferBit (BackBuffer)", &render.clearColour); // Clear BackBuffer
			ImGui::DragInt("Shader Number (Vert)", &shaderStr.VertNum), ImGui::DragInt("Shader Number (Frag)", &shaderStr.FragNum); // Shader Switching

			if (ImGui::SmallButton("Apply Shader?")) { shaderProgramT.Delete(); TempButton = -1; } // apply shader
			ImGui::SliderInt("doReflections", &render.doReflections, 0, 2), ImGui::SliderInt("doFog", &render.doFog, 0, 1); 		//Toggles
			ImGui::TreePop();// Ends The ImGui Window
		}
		// Lighting panel
		if (ImGui::TreeNode("Lighting")) {

			if (ImGui::TreeNode("Colour")) {
				ImGui::ColorEdit4("sky RGBA", skyRGBA), ImGui::ColorEdit4("light RGBA", lightRGBA), ImGui::ColorEdit4("fog RGBA", fogRGBA);	// sky and light
				ImGui::TreePop();}

			if (ImGui::TreeNode("Light Settings")) {
				ImGui::DragFloat("light I", &ConeSI[2]);

				// cone settings
				ImGui::Text("cone size");
				ImGui::SliderFloat("cone Size (D: 0.95)", &ConeSI[1], 0.0f, 1.0f), ImGui::SliderFloat("cone Strength (D: 0.05)", &ConeSI[0], 0.0f, 0.90f);

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
			ImGui::TreePop();}// Ends The ImGui Window 

		if (ImGui::TreeNode("Transform")) {
			if (ImGui::SmallButton("Reset Camera")) { TempButton = 1; } // reset cam pos
			ImGui::DragFloat3("Camera Transform", CameraXYZ); // set cam pos
			if (ImGui::SmallButton("Set")) { TempButton = 2; } // apply cam pos
			ImGui::TreePop();}// Ends The ImGui Window 


		ImGui::TreePop();// Ends The ImGui Window
	}
	ImGui::End();
	
	ImGui::Render(); // Renders the ImGUI elements

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
// Holds DeltaTime Based Variables and Functions
void DeltaMain(GLFWwindow* window) {
	
	// Calculate delta time
	// Cast the value to float
	float currentFrameTime = static_cast<float>(glfwGetTime());
	deltaTimeStr.deltaTime = currentFrameTime - deltaTimeStr.lastFrameTime;
	deltaTimeStr.lastFrameTime = currentFrameTime;

	//framerate tracking
	deltaTimeStr.frameRateI = 1.0f / deltaTimeStr.deltaTime;
	timeAccumulator[0] += deltaTimeStr.deltaTime;
	//1hz
	if (timeAccumulator[0] >= 1.0f) { 	//run if after 1 second
		deltaTimeStr.frameRate1IHZ = 1.0f / deltaTimeStr.deltaTime; //update frameRate1IHZ at 1hz  
		deltaTimeStr.framerate = "FPS " + std::to_string(deltaTimeStr.frameRateI);
		timeAccumulator[0] = 0.0f; //reset time
	}
	timeAccumulator[1] += deltaTimeStr.deltaTime;
	//60hz
	if (timeAccumulator[1] >= 0.016f) { //run if after .16 second

		
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_5) == GLFW_PRESS) { cameraSettings[0] += 0.4f; } // zoom out
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_4) == GLFW_PRESS) { cameraSettings[0] -= 0.4f; } // zoom in
		if (cameraSettings[0] <= 0.00f) { cameraSettings[0] = 0.1f; } // 0.1f zoom

		if (cameraSettings[0] >= 160.1f) { cameraSettings[0] = 160.0f; } // 160.0f zoom

		timeAccumulator[1] = 0.0f; //reset time
	}
	timeAccumulator[2] += deltaTimeStr.deltaTime;
	
	//1000.0f / (5.0f * 1000.0f)) (5hz)
	
	switch (deltaTimeStr.aqFPS) { //graph high correct
	case true:{ //sharp
		if (timeAccumulator[2] >= (1000.0f / (deltaTimeStr.frameRateI * (deltaTimeStr.deltaTime * 1000.0f)))) {
			deltaTimeStr.ftDif = (deltaTimeStr.frameRateI + (deltaTimeStr.frameRateI / 2));
			timeAccumulator[2] = 0.0f; //reset time
		}
			break;
		}
	case false:{ //smooth
		if (timeAccumulator[2] >= (1000.0f / (deltaTimeStr.frameRateI * (10.0f)))) {
			deltaTimeStr.ftDif = (deltaTimeStr.frameRateI + (deltaTimeStr.frameRateI / 2));
			timeAccumulator[2] = 0.0f; //reset time
		}
			break;
		}
	}

}
//Main Function
int main()
{
		
		initializeGLFW(); //initialize glfw

		// Get the video mode of the primary monitor
		// Get the primary monitor
		GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
		if (!primaryMonitor) {
			std::cerr << "Failed to get primary monitor" << std::endl;
			glfwTerminate();
			return -1;
		}

		// Get the video mode of the primary monitor
		const GLFWvidmode* videoMode = glfwGetVideoMode(primaryMonitor);
		if (!videoMode) {
			std::cerr << "Failed to get video mode" << std::endl;
			glfwTerminate();
			return -1;
		}

		// second fallback
		// Store the width and height in the test array
		screen.width = videoMode->width;
		screen.height = videoMode->height;

		// Now call glfwGetMonitorPos with correct arguments
		glfwGetMonitorPos(glfwGetPrimaryMonitor(), &screen.widthI, &screen.heightI);
		loadSettings();

		//    GLFWwindow* window = glfwCreateWindow(videoMode->width, videoMode->height, "Farquhar Engine OPEN GL - 1.3", primaryMonitor, NULL);
		GLFWwindow* window = glfwCreateWindow(videoMode->width, videoMode->height, "Farquhar Engine OPEN GL - 1.4", NULL, NULL); // create window

		// error checking
		if (!window) { std::cerr << "Failed to create window" << std::endl; glfwTerminate(); return -1; } // "Failed to create window"
		if (window == NULL) { std::cout << "failed to create window" << std::endl; glfwTerminate(); return -1; } // "failed to create window"

		glfwMakeContextCurrent(window);	//make window current context
				

		gladLoadGL(); // load open gl config

		//area of open gl we want to render in
		//screen assignment after fallback
		glViewport(0, 0, screen.width, screen.height);
		glfwSetWindowSize(window, screen.width, screen.height);
		std::cout << "Primary monitor resolution: " << screen.width << "x" << screen.height << std::endl;

		// shaderprog init
		Shader shaderProgram("Shaders/Empty.shader", "Shaders/Empty.shader"); // create a shader program and feed it Dummy shader and vertex files
		shaderProgram.Delete(); // clean the shader prog for memory management
		loadShaderProgram(shaderStr.VertNum, shaderStr.FragNum, shaderProgram);// feed the shader prog real data
		shaderProgram.Activate(); // activate new shader program for use

		initializeImGui(window); // Initialize ImGUI

		// glenables
		// depth pass. render things in correct order. eg sky behind wall, dirt under water, not random order
		glEnable(GL_DEPTH_TEST); // Depth buffer
		glDepthFunc(GL_LESS);
		glEnable(GL_CULL_FACE); // Culling

		// INITIALIZE CAMERA
		Camera camera(screen.width, screen.height, glm::vec3(0.0f, 0.0f, 50.0f)); 	// camera ratio pos
		camera.Position = glm::vec3(CameraXYZ[0], CameraXYZ[1], CameraXYZ[2]); // camera ratio pos
		// texture loading problems

		// Model Loader
		std::vector<Model> models = loadModels(mapName + "ModelNames.cfg", mapName + "ModelPaths.cfg"); // Load models from files

		// change window icon
		// Icon Creation
		int iconW, iconH; // Width and Depth
		int iconChannels; // Image number (1)
		stbi_set_flip_vertically_on_load(false); // Disable Image Flipping On Load
		unsigned char* pixelsIcon = stbi_load("assets/Icons/Icon60B.png", &iconW, &iconH, &iconChannels, STBI_rgb_alpha); // create var with imnage inside

		GLFWimage Iconinages[1]; // Create New "GLFWimage" VAR with "Iconinages at Channel (1)"
		Iconinages[0].width = iconW, Iconinages[0].height = iconH, Iconinages[0].pixels = pixelsIcon; // Write Aspect Ratio and Fragnment to photo 

		glfwSetWindowIcon(window, 1, Iconinages); // set the glfw window icon ("window", "Channel", "Image")

		setVSync(render.doVsync); // Set Vsync to value of doVsync (bool)

		while (!glfwWindowShouldClose(window)) // GAME LOOP
		{
			DeltaMain(window); // Calls the DeltaMain Method that Handles variables that require delta time (FrameTime, FPS, ETC) 

			switch (TempButton) {
			case -1: {
				loadShaderProgram(shaderStr.VertNum, shaderStr.FragNum, shaderProgram);
				TempButton = 0; break; }
			case 1: {
				camera.Position = glm::vec3(0, 0, 0);
				TempButton = 0; break; }
			case 2: {
				camera.Position = glm::vec3(CameraXYZ[0], CameraXYZ[1], CameraXYZ[2]);
				TempButton = 0; break; }
			}

			//Clear BackBuffer
			if (render.clearColour) { glClear(GL_DEPTH_BUFFER_BIT); } // clear just depth buffer for lols
			else { glClearColor(skyRGBA[0], skyRGBA[1], skyRGBA[2], skyRGBA[3]), glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); } 	// Clear with colour


			// Convert variables to glm variables which hold data like a table
			glm::vec3 lightPos = glm::vec3(LightTransform1[0], LightTransform1[1], LightTransform1[2]);

			glm::mat4 lightModel = glm::mat4(1.0f); lightModel = glm::translate(lightModel, lightPos);

			//Send Variables to shader (GPU)
			shaderProgram.Activate(); // activate shaderprog to send vars to gpu

			//DO/1f
			glUniform1i(glGetUniformLocation(shaderProgram.ID, "doReflect"), render.doReflections), glUniform1i(glGetUniformLocation(shaderProgram.ID, "doFog"), render.doFog);

			//3f
			glUniform3f(glGetUniformLocation(shaderProgram.ID, "InnerLight1"), (ConeSI[1] - ConeSI[0]), ConeSI[1], ConeSI[2]), 
			glUniform3f(glGetUniformLocation(shaderProgram.ID, "spotLightRot"), ConeRot[0], ConeRot[1], ConeRot[2]), glUniform3f(glGetUniformLocation(shaderProgram.ID, "fogColor"), fogRGBA[0], fogRGBA[1], fogRGBA[2]),
			glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), LightTransform1[0], LightTransform1[1], LightTransform1[2]);

			//4f
			glUniform4f(glGetUniformLocation(shaderProgram.ID, "skyColor"), skyRGBA[0], skyRGBA[1], skyRGBA[2], skyRGBA[3]),
			glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightRGBA[0], lightRGBA[1], lightRGBA[2], lightRGBA[3]);


			//Camera
			camera.Inputs(window, deltaTimeStr.deltaTime); //send Camera.cpp window inputs and delta time
			camera.updateMatrix(cameraSettings[0], cameraSettings[1], cameraSettings[2]); // Update: fov, near and far plane

			for (Model& model : models) { model.Draw(shaderProgram, camera); }// draw the model

			camera.Matrix(shaderProgram, "camMatrix"); //Send Camera Matrix To Shader Prog
			imGuiMAIN(window, shaderProgram, primaryMonitor);

			glfwSwapBuffers(window); // Swap BackBuffer with FrontBuffer (DoubleBuffering)
			glfwPollEvents(); // Tells open gl to proccess all events such as window resizing, inputs (KBM)
		}

		// Cleanup: Delete all objects on close
		ImGui_ImplOpenGL3_Shutdown(), ImGui_ImplGlfw_Shutdown(), ImGui::DestroyContext(); // Kill ImGui
		shaderProgram.Delete(); // Delete Shader Prog
		glfwDestroyWindow(window), glfwTerminate(); // Kill opengl
		return 0;
}