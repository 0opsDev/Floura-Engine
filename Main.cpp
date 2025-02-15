#include"src/video/Model.h"
#include"imgui.h"
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <vector>
#include"imgui_impl_glfw.h"
#include"imgui_impl_opengl3.h"

// these two where const
// 
// 
// 
// 
//refactoring, namespaces, encapulation, classes


// W+H
//FALLBACK
unsigned int screenArea[2] = { 800, 600 };
int screenAreaI[2] = { screenArea[0], screenArea[1] };
int frameRateI = 0;

// Initialize previous time and delta time
float lastFrameTime = 0.0f;
float deltaTime = 0.0f;
// 1hz 60hz
static float timeAccumulator[2] = { 0.0f, 0.0f };

GLfloat ConeSI[3] = { 0.05f, 0.95f , 1.0f };
GLfloat ConeRot[3] = { 0.0f, -4.0f , 0.0f };
GLfloat LightTransform1[3] = { 0.0f, 25.0f, 0.0f };
GLfloat lightRGBA[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat skyRGBA[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat fogRGBA[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat CameraXYZ[3] = { 0.0f, 5.0f, 0.0f };
//FOV , near, far
float cameraSettins[3] = { 60.0f, 0.1f, 1000.0f };
bool doVsync = false;
bool clearColour = false;
int doReflections = 1;
int doFog = 1;
//Render, Camera, Light
bool Panels[4] = { true, true, true, true };
bool CapFps = false;
int TempButton = 0;
int VertNum = 0;
int FragNum = 2;
std::string framerate;
float ftDif;
std::string mapName = "";
bool isFullscreen = false;
int windowedPosX, windowedPosY, windowedWidth, windowedHeight;
bool checkboxVar[1] = { false };

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

void setVSync(bool enabled) {
	glfwSwapInterval(enabled ? 1 : 0);
}

void loadSettings() {
	//READ - settings
	std::ifstream TestFile2("Settings/Settings.ini");
	if (TestFile2.is_open())
	{
		std::string lineT;
		int lineNumber = 0;

		while (std::getline(TestFile2, lineT)) {
			lineNumber++;
			std::istringstream iss(lineT);
			GLfloat value;

			switch (lineNumber) {
			case 3:
				if (iss >> value) screenArea[0] = static_cast<unsigned int>(value);
				break;
			case 4:
				if (iss >> value) screenArea[1] = static_cast<unsigned int>(value);
				break;
			case 6:
				doVsync = (lineT == "VsyncT");
				std::cout << "Vsync: " << doVsync << std::endl;
				break;
			case 8:
				if (iss >> value) cameraSettins[0] = value;
				std::cout << "Camera FOV: " << cameraSettins[0] << std::endl;
				break;
			default:
				break;
			}
			screenAreaI[0] = screenArea[0];
			screenAreaI[1] = screenArea[1];
		}
		TestFile2.close();

		std::ifstream TestFile3("Settings/Engine.ini");
		if (TestFile3.is_open())
		{
			lineNumber = 0;

			while (std::getline(TestFile3, lineT)) {
				lineNumber++;
				std::istringstream iss(lineT);
				GLfloat value;

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
					if (iss >> value) cameraSettins[1] = value;
					std::cout << "Camera Near Plane: " << cameraSettins[1] << std::endl;
					break;
				case 6:
					if (iss >> value) cameraSettins[2] = value;
					std::cout << "Camera Far Plane: " << cameraSettins[2] << std::endl;
					break;
				case 3:
					mapName = "Assets/Maps/" + lineT + "/";
					std::cout << "Map: " << mapName << std::endl;
					break;
				default:
					break;
				}
			}
			TestFile3.close();
		}
	}
}

void initializeGLFW() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, 1);
	glfwWindowHint(GLFW_MAXIMIZED, 1);
	glfwWindowHint(GLFW_DEPTH_BITS, 16);

}

void initializeImGui(GLFWwindow* window) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
}
void toggleFullscreen(GLFWwindow* window, GLFWmonitor* monitor) {
	isFullscreen = !isFullscreen;
	if (isFullscreen) {
		// Save windowed mode dimensions and position
		glfwGetWindowPos(window, &windowedPosX, &windowedPosY);
		glfwGetWindowSize(window, &windowedWidth, &windowedHeight);

		// Get the video mode of the monitor
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);

		// Switch to fullscreen
		glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
	}
	else {
		// Switch to windowed mode
		glfwSetWindowMonitor(window, NULL, windowedPosX, windowedPosY, windowedWidth, windowedHeight, 0);
	}
}

void imGuiMAIN(GLFWwindow* window, Shader shaderProgramT, GLFWmonitor* monitorT) {
	// ImGUI window creation
	ImGui::Begin("Settings");
	ImGui::Text("Settings (Press escape to use mouse)");
	//load settings button
	if (ImGui::SmallButton("load")) {
		loadSettings();
	}
	//ImGui::Checkbox("save changes?", &save);
	ImGui::Checkbox("Rendering Panel", &Panels[0]);
	ImGui::Checkbox("Camera Panel", &Panels[1]);
	ImGui::Checkbox("Lighting Panel", &Panels[2]);
	ImGui::Checkbox("Preformance Profiler", &Panels[3]);


	// Ends the window
	ImGui::End();
	//Rendering panel
	if (Panels[0]) {
		ImGui::Begin("Rendering");

		ImGui::Text("Framerate Limiters");
		ImGui::Checkbox("Vsync", &doVsync);
		//rendering

		//screen res
		ImGui::DragInt("Width", &screenAreaI[0]);
		ImGui::DragInt("Height", &screenAreaI[1]);
		//apply button
		if (ImGui::SmallButton("Apply Changes?")) {

			screenArea[0] = static_cast<unsigned int>(screenAreaI[0]);
			screenArea[1] = static_cast<unsigned int>(screenAreaI[1]);
			glViewport(0, 0, screenArea[0], screenArea[1]);
			glfwSetWindowSize(window, screenArea[0], screenArea[1]);

			setVSync(doVsync);
		}
		if (ImGui::SmallButton("Toggle Fullscreen (WARNING WILL TOGGLE HDR OFF)")) {

			toggleFullscreen(window, monitorT);
		}
		if (ImGui::SmallButton("Enable Culling")) {
			glEnable(GL_CULL_FACE);
		}
		if (ImGui::SmallButton("Disable Culling")) {
			glDisable(GL_CULL_FACE);
		}

		ImGui::Checkbox("ClearColourBufferBit (BackBuffer)", &clearColour);
		ImGui::DragInt("Shader Number (Vert)", &VertNum);
		ImGui::DragInt("Shader Number (Frag)", &FragNum);
		if (ImGui::SmallButton("Apply Shader?")) {
			shaderProgramT.Delete();
			TempButton = -1;
		}
		ImGui::End();
	}

	//Camera panel
	if (Panels[1]) {

		ImGui::Begin("Camera Settings");

		//Vsync
		//ImGui::Checkbox(igSettings[1], &doVsync);
		//FOV
		ImGui::SliderFloat("FOV", &cameraSettins[0], 0.1f, 160.0f);
		ImGui::DragFloat2("Near and Far Plane", &cameraSettins[1], cameraSettins[2]);

		//reset camera pos
		ImGui::Text("Transform");
		if (ImGui::SmallButton("Reset Camera")) {
			TempButton = 1;
		}
		//set cam pos
		ImGui::DragFloat3("Camera Transform", CameraXYZ);
		if (ImGui::SmallButton("Set")) {
			TempButton = 2;
		}
		ImGui::End();
	}
	//Lighting panel
	if (Panels[2]) {
		ImGui::Begin("Lighting");

		ImGui::Text("Point Light (BROKE)");
		ImGui::DragFloat3("Light Transform", LightTransform1);
		ImGui::Text("Lighting");
		ImGui::Text("Light color and intens");
		//sky and light
		ImGui::ColorEdit4("sky RGBA", skyRGBA);
		ImGui::ColorEdit4("light RGBA", lightRGBA);
		ImGui::ColorEdit4("fog RGBA", fogRGBA);
		ImGui::DragFloat("light I", &ConeSI[2]);
		ImGui::Text("cone size");
		ImGui::SliderFloat("cone Size (D: 0.95)", &ConeSI[1], 0.0f, 1.0f);
		ImGui::SliderFloat("cone Strength (D: 0.05)", &ConeSI[0], 0.0f, 0.90f);
		ImGui::Text("Light Angle");
		ImGui::DragFloat3("Cone Angle", ConeRot);
		ImGui::SliderInt("doReflections", &doReflections, 0, 2);
		ImGui::SliderInt("doFog", &doFog, 0, 1);
		ImGui::End();
	}
	//preformance profiler
	if (Panels[3]) {
		ImGui::Begin("Preformance Profiler");
		// Framerate graph
		static float framerateValues[90] = { 0 };
		static int frValues_offset = 0;
		framerateValues[frValues_offset] = frameRateI;
		frValues_offset = (frValues_offset + 1) % IM_ARRAYSIZE(framerateValues);
		ImGui::Text(framerate.c_str());
		//ftDif = current frame rate(PER SEC) + half of current frame rate so the graph has space to display(max graph height
		ImGui::PlotLines("Framerate (FPS) Graph", framerateValues, IM_ARRAYSIZE(framerateValues), frValues_offset, nullptr, 0.0f, ftDif, ImVec2(0, 80));
		// Frame time graph
		//stores 90 snapshots of frametime
		static float frameTimeValues[90] = { 0 };

		static int ftValues_offset = 0;
		frameTimeValues[ftValues_offset] = deltaTime * 1000.0f; // Convert to milliseconds
		ftValues_offset = (ftValues_offset + 1) % IM_ARRAYSIZE(frameTimeValues);
		std::string frametimes = "Frame Times " + std::to_string(frameTimeValues[ftValues_offset] = deltaTime * 1000.0f) + " ms";
		ImGui::Text(frametimes.c_str());
		ImGui::PlotLines("Frame Times (ms) Graph", frameTimeValues, IM_ARRAYSIZE(frameTimeValues), ftValues_offset, nullptr, 0.0f, 50.0f, ImVec2(0, 80));
		ImGui::End();
	}
	// Renders the ImGUI elements
	ImGui::Render();
}

int main()
{
	//calls the LoadSettings function
	//initialize inside of function
	//start glfw
	initializeGLFW();

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

	//second fallback
	// Store the width and height in the test array
	screenArea[0] = videoMode->width;
	screenArea[1] = videoMode->height;
	// Print the resolution

	// Now call glfwGetMonitorPos with correct arguments
	glfwGetMonitorPos(glfwGetPrimaryMonitor(), &screenAreaI[0], &screenAreaI[1]);
	loadSettings();
	//size, name, fullscreen
	//create window
	//    GLFWwindow* window = glfwCreateWindow(videoMode->width, videoMode->height, "Farquhar Engine OPEN GL - 1.3", primaryMonitor, NULL);
	GLFWwindow* window = glfwCreateWindow(videoMode->width, videoMode->height, "Farquhar Engine OPEN GL - 1.3", NULL, NULL);
	if (!window) {
		std::cerr << "Failed to create window" << std::endl;
		glfwTerminate();
		return -1;
	}

	//error checking
	if (window == NULL)
	{
		std::cout << "failed to create window" << std::endl;
		glfwTerminate();
		return -1;
	}
	//make window current context
	glfwMakeContextCurrent(window);

	//load open gl config
	gladLoadGL();

	//area of open gl we want to render in
	//screen assignment after fallback
	glViewport(0, 0, screenArea[0], screenArea[1]);
	glfwSetWindowSize(window, screenArea[0], screenArea[1]);
	std::cout << "Primary monitor resolution: " << screenAreaI[0] << "x" << screenAreaI[1] << std::endl;

	//create a shader program and feed it Dummy shader and vertex files
	Shader shaderProgram("Shaders/Empty.shader", "Shaders/Empty.shader");

	//clean the shader prog for memory management
	shaderProgram.Delete();
	//feed the shader prog real data
	loadShaderProgram(VertNum, FragNum, shaderProgram);

	shaderProgram.Activate();

	// Initialize ImGUI
	initializeImGui(window);
	//depth pass. render things in correct order. eg sky behind wall, dirt under water, not random order
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//need to do more research into this one
	glEnable(GL_CULL_FACE);

	//INITIALIZE CAMERA
	// camera ratio and pos
	Camera camera(screenArea[0], screenArea[1], glm::vec3(0.0f, 0.0f, 50.0f));
	camera.Position = glm::vec3(CameraXYZ[0], CameraXYZ[1], CameraXYZ[2]);
	//texture loading problems
	// Load models from files
	std::vector<Model> models = loadModels(mapName + "ModelNames.cfg", mapName + "ModelPaths.cfg");

	//Model model5("Assets/Models/test/test.gltf");
	//icon creation
	int iconW, iconH;
	int iconChannels;
	//STBI_rgb_alpha

	unsigned char* pixelsIcon = stbi_load("assets/Icons/Icon60B-F.png", &iconW, &iconH, &iconChannels, STBI_rgb_alpha);

	//change window icon
	GLFWimage Iconinages[1];
	//give glfw pixels, width and height
	Iconinages[0].width = iconW;
	Iconinages[0].height = iconH;
	Iconinages[0].pixels = pixelsIcon;

	//change to icon (what window, how many images, what image)
	glfwSetWindowIcon(window, 1, Iconinages);
	//glfwCreateCursor(Iconinages, iconW, iconH);
	setVSync(doVsync);
	//game loop
	//makes sure window stays open
	while (!glfwWindowShouldClose(window))
	{

		switch (TempButton) {
		case -1: {
			loadShaderProgram(VertNum, FragNum, shaderProgram);
			TempButton = 0;
			break;
		}
		case 1: {
			camera.Position = glm::vec3(0, 0, 0);
			TempButton = 0;
			break;
		}
		case 2: {
			camera.Position = glm::vec3(CameraXYZ[0], CameraXYZ[1], CameraXYZ[2]);
			TempButton = 0;
			break;
		}
		}
		// Calculate delta time
		// Cast the value to float

		float currentFrameTime = static_cast<float>(glfwGetTime());
		deltaTime = currentFrameTime - lastFrameTime;
		lastFrameTime = currentFrameTime;

		//framerate tracking
		frameRateI = 1.0f / deltaTime;

		timeAccumulator[0] += deltaTime;
		//1hz
		if (timeAccumulator[0] >= 1.0f) {
			//run if after 1 second
			ftDif = (frameRateI + (frameRateI / 2));
			framerate = "FPS " + std::to_string(frameRateI);
			timeAccumulator[0] = 0.0f;
		}
		timeAccumulator[1] += deltaTime;
		//60hz
		if (timeAccumulator[1] >= 0.016f) {
			//run if after .16 second
			if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_5) == GLFW_PRESS)
			{
				cameraSettins[0] += 0.4f;
			}
			if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_4) == GLFW_PRESS)
			{
				cameraSettins[0] -= 0.4f;
			}
			if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
			{
				cameraSettins[0] = 60.0f;
			}
			if (cameraSettins[0] <= 0.00f)
			{
				cameraSettins[0] = 0.1f;
			}
			if (cameraSettins[0] >= 160.1f)
			{
				cameraSettins[0] = 160.0f;
			}
			timeAccumulator[1] = 0.0f;
		}

		//Tell OpenGL a new frame is about to begin
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		//CLEAR BACK BUFFER
		//RGB ALPHA please re enable to fix sky colour
		// SEND TO COLOR BUFFER (DRAWS COLOUR TO SCREEN)
		//glclear only needs to run once btw
		if (clearColour) {
			glClear(GL_DEPTH_BUFFER_BIT);
		}
		else {
			glClearColor(skyRGBA[0], skyRGBA[1], skyRGBA[2], skyRGBA[3]);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		//inputs

		camera.Inputs(window, deltaTime);

		// camera fov, near and far plane
		camera.updateMatrix(cameraSettins[0], cameraSettins[1], cameraSettins[2]);

		// draw the model
		for (Model& model : models) {
			model.Draw(shaderProgram, camera);
		}

		glm::vec3 lightPos = glm::vec3(LightTransform1[0], LightTransform1[1], LightTransform1[2]);
		glm::mat4 lightModel = glm::mat4(1.0f);
		lightModel = glm::translate(lightModel, lightPos);

		shaderProgram.Activate();

		glUniform1i(glGetUniformLocation(shaderProgram.ID, "doReflect"), doReflections);
		glUniform1i(glGetUniformLocation(shaderProgram.ID, "doFog"), doFog);
		glUniform3f(glGetUniformLocation(shaderProgram.ID, "InnerLight1"), (ConeSI[1] - ConeSI[0]), ConeSI[1], ConeSI[2]);
		glUniform3f(glGetUniformLocation(shaderProgram.ID, "spotLightRot"), ConeRot[0], ConeRot[1], ConeRot[2]);
		glUniform4f(glGetUniformLocation(shaderProgram.ID, "skyColor"), skyRGBA[0], skyRGBA[1], skyRGBA[2], skyRGBA[3]);
		glUniform3f(glGetUniformLocation(shaderProgram.ID, "fogColor"), fogRGBA[0], fogRGBA[1], fogRGBA[2]);
		// update light color seprate from the model
		glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightRGBA[0], lightRGBA[1], lightRGBA[2], lightRGBA[3]);
		// update light pos
		glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), LightTransform1[0], LightTransform1[1], LightTransform1[2]);

		camera.Matrix(shaderProgram, "camMatrix");

		// Render ImGUI elements
		imGuiMAIN(window, shaderProgram, primaryMonitor);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Swap back buffer with front buffer
		glfwSwapBuffers(window);
		// Tells open gl to proccess all events like window resizing and all otheer events
		glfwPollEvents();
	}
	// Cleanup
	// Delete all objects on close
	// Deletes all ImGUI instances
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	shaderProgram.Delete();
	// kill opengl
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}