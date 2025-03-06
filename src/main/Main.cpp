#include"Model.h"
#include "Camera.h"
#include "Main.h"
#include "UF.h"
#include "Init.h"
#include "screenutils.h" 

//Global Variables

//Render
struct RenderSettings { int doReflections = 1, doFog = 1; bool doVsync = false, clearColour = false, frontFaceSide = false; }; RenderSettings render;

//Shader
struct ShaderSettings { int VertNum = 0, FragNum = 2; bool Stencil = 0; float stencilSize = 0.009f, stencilColor[4] = {1.0f, 1.0f, 1.0f, 1.0f}, gamma = 2.2; };
//GLfloat, Render, Camera, Light
GLfloat ConeSI[3] = { 0.111f, 0.825f , 2.0f }, ConeRot[3] = { 0.0f, -1.0f , 0.0f },
LightTransform1[3] = { 0.0f, 5.0f, 0.0f }, CameraXYZ[3] = { 0.0f, 5.0f, 0.0f },
lightRGBA[4] = { 0.0f, 0.0f, 0.0f, 1.0f }, skyRGBA[4] = { 1.0f, 1.0f, 1.0f, 1.0f },
fogRGBA[4] = { 1.0f, 1.0f, 1.0f, 1.0f }, DepthDistance = 100.0f, DepthPlane[2] = {0.1f, 100.0f};
ShaderSettings shaderStr;

struct ScreenSettings { //Screen
	unsigned int width = 800, height = 600;
	int windowedPosX, windowedPosY, windowedWidth, windowedHeight, widthI, heightI;
	bool isFullscreen = false;
	std::string WindowTitle = "OpenGL Window";
}; ScreenSettings screen = {0};

struct DeltaTime { //DeltaTime
	int frameRateI, frameRate1IHZ;
	float lastFrameTime, deltaTime, ftDif;
	bool aqFPS = true;
	std::string framerate;
}; DeltaTime deltaTimeStr = {0};
static float timeAccumulator[3] = { 0.0f, 0.0f, 0.0f }; // DeltaTime Accumulators

int TempButton = 0;
bool Panels[3] = { true, true, true}; // ImGui Panels

float cameraSettings[3] = { 60.0f, 0.1f, 1000.0f }; // Float, DeltaTime, Camera: FOV , near, far

std::string mapName = ""; // String, Maploading

// Function to read lines from a file into a vector of strings
std::vector<std::string> readLinesFromFile(const std::string& filePath) {
	// Shaders
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
std::vector<int> readCullingSettings(const std::string& filePath) {
	std::vector<int> cullingSettings;
	std::ifstream file(filePath);
	if (file.is_open()) {
		std::string line;
		while (std::getline(file, line)) {
			cullingSettings.push_back(std::stoi(line));
		}
		file.close();
	}
	else {
		std::cerr << "Failed to open file: " << filePath << std::endl;
	}
	return cullingSettings;
}



// Function to load models from files
std::vector<std::pair<Model, int>> loadModels(const std::string& namesFilePath, const std::string& pathsFilePath, const std::string& cullingFilePath) {
	std::vector<std::string> modelNames = readLinesFromFile(namesFilePath);
	std::vector<std::string> modelPaths = readLinesFromFile(pathsFilePath);
	std::vector<int> cullingSettings = readCullingSettings(cullingFilePath);

	std::vector<std::pair<Model, int>> models;

	if (modelNames.size() != modelPaths.size() || modelNames.size() != cullingSettings.size()) {
		std::cout << "\n ERR: Model names, paths, and culling settings count mismatch" << std::endl;
		models.emplace_back(Model("Assets/assets/fallback/model/placeholder/placeholder.gltf"), 0);
	}
	else {
		for (size_t i = 0; i < modelNames.size(); ++i) {
			models.emplace_back(Model((mapName + modelPaths[i]).c_str()), cullingSettings[i]);
			std::cout << "Loaded model: " << '"' << modelNames[i] << '"' << " from path: " << modelPaths[i] << std::endl;
		}
	}
	return models;
}



//Methods
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
				case 20:
					screen.WindowTitle = lineT;
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
		std::ifstream TestFile4("Settings/imguiPanels.ini");
		if (TestFile4.is_open())
		{
			lineNumber = 0;

			while (std::getline(TestFile4, lineT)) {
				lineNumber++;
				std::istringstream iss(lineT); GLfloat value; // variable init

				switch (lineNumber) {
				case 3:
					if (iss >> value) Panels[0] = value;
					std::cout << "ImGui: " << Panels[0] << std::endl;
					break;
				case 6:
					if (iss >> value) Panels[1] = value;
					std::cout << "Main Panel: " << Panels[1] << std::endl;
					break;
				case 8:
					if (iss >> value) Panels[2] = value;
					std::cout << "Preformance Panel: " << Panels[2] << std::endl;
					break;
				default:
					break;
				}
			}
			TestFile4.close();
		}
	}
}
// Holds ImGui Variables and Windows
void imGuiMAIN(GLFWwindow* window, Shader shaderProgramT, GLFWmonitor* monitorT, ScreenUtils ScreenH) {
	//Tell Imgui a new frame is about to begin
	ImGui_ImplOpenGL3_NewFrame();ImGui_ImplGlfw_NewFrame(); ImGui::NewFrame();
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

				if (ImGui::SmallButton("Apply Changes?")) { // apply button
					screen.width = static_cast<unsigned int>(screen.widthI);
					screen.height = static_cast<unsigned int>(screen.heightI); // cast screenArea from screenAreaI
					ScreenH.SetScreenSize(window, screen.width, screen.height); // set window and viewport w&h
					ScreenH.setVSync(render.doVsync); // Set Vsync to value of doVsync (bool)
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
		frameTimeValues[ftValues_offset] = deltaTimeStr.deltaTime * 1000.0f; // Convert to milliseconds
		ftValues_offset = (ftValues_offset + 1) % IM_ARRAYSIZE(frameTimeValues);
		std::string frametimes = "Frame Times " + std::to_string(frameTimeValues[ftValues_offset] = deltaTimeStr.deltaTime * 1000.0f) + " ms";

		ImGui::Text(frametimes.c_str());
		ImGui::PlotLines("Frame Times (ms) Graph (90SAMP)", frameTimeValues, IM_ARRAYSIZE(frameTimeValues), ftValues_offset, nullptr, 0.0f, 50.0f, ImVec2(0, 80));
		ImGui::End();
	}
	
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
		glfwSetWindowTitle(window, (screen.WindowTitle + " (FPS:" + std::to_string(deltaTimeStr.frameRateI) + ")").c_str()); //set window title to framerate
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
		init init; init.initGLFW(); // initialize glfw
		ScreenUtils ScreenH;

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

		//area of open gl we want to render in
		//screen assignment after fallback
		ScreenH.SetScreenSize(window, screen.width, screen.height);  // set window and viewport w&h

		std::cout << "Primary monitor resolution: " << screen.width << "x" << screen.height << std::endl;

		UF UniformH; // glunfiorm

		// shaderprog init
		Shader shaderProgram("Shaders/Empty.shader", "Shaders/Empty.shader"); // create a shader program and feed it Dummy shader and vertex files
		shaderProgram.Delete(); // clean the shader prog for memory management
		loadShaderProgram(shaderStr.VertNum, shaderStr.FragNum, shaderProgram);// feed the shader prog real data
		shaderProgram.Activate(); // activate new shader program for use

		Shader outlineShaderProgram("Shaders/Main/outlining.vert", "Shaders/Main/outlining.frag");
		Shader LightProgram("Shaders/Db/light.vert", "Shaders/Db/light.frag");

		init.initImGui(window); // Initialize ImGUI
		
		if (Panels[0]) { imGuiMAIN(window, shaderProgram, primaryMonitor, ScreenH); }

		// glenables
		// depth pass. render things in correct order. eg sky behind wall, dirt under water, not random order
		init.initGLenable(render.frontFaceSide);
		
		// INITIALIZE CAMERA
		Camera camera(screen.width, screen.height, glm::vec3(0.0f, 0.0f, 50.0f)); 	// camera ratio pos
		camera.Position = glm::vec3(CameraXYZ[0], CameraXYZ[1], CameraXYZ[2]); // camera ratio pos

		// texture loading problems

		// Model Loader
		std::vector<std::pair<Model, int>> models = loadModels(mapName + "ModelNames.cfg", mapName + "ModelPaths.cfg", mapName + "ModelCull.cfg"); // Load models from files

		Model Lightmodel = "Assets/assets/Light/light.gltf";

		// window logo creation and assignment
		init.initLogo(window);

		ScreenH.setVSync(render.doVsync); // Set Vsync to value of doVsync (bool)

		while (!glfwWindowShouldClose(window)) // GAME LOOP
		{
			if (glfwGetKey(window, GLFW_KEY_HOME) == GLFW_PRESS) { loadSettings(); }

			DeltaMain(window); // Calls the DeltaMain Method that Handles variables that require delta time (FrameTime, FPS, ETC) 

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
			LightProgram.Activate();
			UniformH.Float4(LightProgram.ID, "lightColor", lightRGBA[0], lightRGBA[1], lightRGBA[2], lightRGBA[3]);
			UniformH.Float3(LightProgram.ID, "Lightmodel", lightPos.x, lightPos.y, lightPos.z);

			// Camera
			camera.Inputs(window, deltaTimeStr.deltaTime); // send Camera.cpp window inputs and delta time
			camera.updateMatrix(cameraSettings[0], cameraSettings[1], cameraSettings[2]); // Update: fov, near and far plane


			// Clear BackBuffer
			if (render.clearColour) { glClear(GL_DEPTH_BUFFER_BIT); } // clear just depth buffer for lols
			else { glClearColor(skyRGBA[0], skyRGBA[1], skyRGBA[2], skyRGBA[3]), glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); } // Clear with colour

			// draw the model
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilMask(0xFF);

			for (auto& modelPair : models) {
				Model& model = modelPair.first;
				int cullingSetting = modelPair.second;

				// Apply culling settings
				if (cullingSetting == 1) {
					glEnable(GL_CULL_FACE);
				}
				else {
					glDisable(GL_CULL_FACE);
				}

				model.Draw(shaderProgram, camera);
			}

			glDisable(GL_CULL_FACE);
			Lightmodel.Draw(LightProgram, camera);

			if (shaderStr.Stencil) {
				glStencilFunc(GL_NOTEQUAL, 1, 0XFF);
				glStencilMask(0x00);
				glDisable(GL_DEPTH_TEST);
				outlineShaderProgram.Activate();
				UniformH.Float(outlineShaderProgram.ID, "outlining", shaderStr.stencilSize);
				UniformH.Float4(outlineShaderProgram.ID, "stencilColor", shaderStr.stencilColor[0], shaderStr.stencilColor[1], shaderStr.stencilColor[2], shaderStr.stencilColor[3]);
				// add stencil buffer toggle tommorow
				// draw
				for (auto& modelPair : models) {
					Model& model = modelPair.first;
					model.Draw(outlineShaderProgram, camera);
				}

				glStencilMask(0xFF);
				glStencilFunc(GL_ALWAYS, 0, 0xFF);
				glEnable(GL_DEPTH_TEST);
			}

			camera.Matrix(shaderProgram, "camMatrix"); // Send Camera Matrix To Shader Prog
			camera.Matrix(LightProgram, "camMatrix"); // Send Camera Matrix To Shader Prog

			if (Panels[0]) { imGuiMAIN(window, shaderProgram, primaryMonitor, ScreenH); }

			glfwSwapBuffers(window); // Swap BackBuffer with FrontBuffer (DoubleBuffering)
			glfwPollEvents(); // Tells open gl to proccess all events such as window resizing, inputs (KBM)
		}
		// Cleanup: Delete all objects on close

		ImGui_ImplOpenGL3_Shutdown(), ImGui_ImplGlfw_Shutdown(), ImGui::DestroyContext(); // Kill ImGui
		shaderProgram.Delete(); // Delete Shader Prog
		outlineShaderProgram.Delete();
		glfwDestroyWindow(window), glfwTerminate(); // Kill opengl
		return 0;
}