#include"src/video/Model.h"
#include"imgui.h"
#include <fstream>
#include <stdexcept>
#include <iostream>
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

GLfloat ConeSI[3] = { 0.05f, 0.95f , 1.0f }; //currently useless
GLfloat ConeRot[3] = { 1.0f, -1.0f , 0.0f }; //currently useless
GLfloat LightTransform1[3] = { 0.0f, 25.0f, 0.0f }; //currently useless
GLfloat varFOV = 60.0f;
GLfloat lightRGBA[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat skyRGBA[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat CameraXYZ[3] = { 0.0f, 0.0f, 50.0f };
//FOV , near, far
float cameraSettins[3] = { 60.0f, 0.1f, 1000.0f };
bool doVsync = false;
bool clearColour = false;
int doReflections = 1;
//Render, Camera, Light
bool Panels[3] = { true, true, true };
int TempButton = 0;
int ShaderNum = 1;
std::string framerate;

const char* igSettings[] =
{
	"Settings Window" , "Vsync", "FOV",
	"Reset Camera",
	"sky RGBA", "light RGBA", "Camera Transform",
	"Near and Far Plane"
};
const char* igTex[] =
{
	"Settings (Press escape to use mouse)" , "Rendering",
	"Transform", "Lighting", "Light color and intens", "Camera Settings"
};

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
				if (iss >> value) skyRGBA[0] = value;
				break;
			case 4:
				if (iss >> value) skyRGBA[1] = value;
				break;
			case 5:
				if (iss >> value) skyRGBA[2] = value;
				break;
			case 7:
				if (iss >> value) lightRGBA[0] = value;
				break;
			case 8:
				if (iss >> value) lightRGBA[1] = value;
				break;
			case 9:
				if (iss >> value) lightRGBA[2] = value;
				break;
			case 11:
				if (iss >> value) screenArea[0] = value;
				break;
			case 12:
				if (iss >> value) screenArea[1] = value;
				break;
			case 14:
				doVsync = (lineT == "VsyncT");
				std::cout << "Vsync: " << doVsync << std::endl;
				break;
			case 16:
				if (iss >> value) cameraSettins[0] = value;
				std::cout << "Camera FOV: " << cameraSettins[0] << std::endl;
				break;
			case 18:
				if (iss >> value) cameraSettins[1] = value;
				std::cout << "Camera Near Plane: " << cameraSettins[1] << std::endl;
				break;
			case 19:
				if (iss >> value) cameraSettins[2] = value;
				std::cout << "Camera Far Plane: " << cameraSettins[2] << std::endl;
				break;
			default:
				break;
			}
			screenAreaI[0] = screenArea[0];
			screenAreaI[1] = screenArea[1];
		}
		TestFile2.close();

		//array of settings files which determines what line to read on the ini fale

//	std::fstream TestFile;
//	TestFile.open("Settings/DoDefaultAnimation.txt", std::ios::out);//write
//	if (TestFile.is_open()) {
//		if (DoDefaultAnimation) {
//			TestFile << "true\n";
//			std::cout << "true\n";
//		}
//		else {
//			TestFile << "false\n";
//			std::cout << "false\n";
//		}
//	}
	}
};

void initializeGLFW() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, 1);
	glfwWindowHint(GLFW_MAXIMIZED, 1);
}

void initializeImGui(GLFWwindow* window) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
}

void imGuiMAIN(GLFWwindow* window) {
	// ImGUI window creation
	ImGui::Begin(igSettings[0]);
	ImGui::Text(igTex[0]);
	ImGui::Text(framerate.c_str());
	//load settings button
	if (ImGui::SmallButton("load")) {
		loadSettings();
	}
	//ImGui::Checkbox("save changes?", &save);
	ImGui::Checkbox("Rendering Panel", &Panels[0]);
	ImGui::Checkbox("Camera Panel", &Panels[1]);
	ImGui::Checkbox("Lighting Panel", &Panels[2]);
	// Ends the window
	ImGui::End();
	//Rendering panel
	if (Panels[0]) {
		ImGui::Begin(igTex[1]);

		ImGui::Checkbox(igSettings[1], &doVsync);
		//rendering
		//do vsync
		//screen res
		ImGui::DragInt("Width", &screenAreaI[0]);
		ImGui::DragInt("Height", &screenAreaI[1]);
		//apply button
		if (ImGui::SmallButton("Apply Changes?")) {

			screenArea[0] = screenAreaI[0];
			screenArea[1] = screenAreaI[1];
			glViewport(0, 0, screenArea[0], screenArea[1]);
			glfwSetWindowSize(window, screenArea[0], screenArea[1]);

			setVSync(doVsync);
		}
		if(ImGui::SmallButton("Enable Culling")) {
			glEnable(GL_CULL_FACE);
		}
		if(ImGui::SmallButton("Disable Culling")) {
			glDisable(GL_CULL_FACE);
		}

		ImGui::Checkbox("ClearColourBufferBit (BackBuffer)", &clearColour);
		ImGui::DragInt("Shader Number (Frag)", &ShaderNum);
		ImGui::End();
	}
	//Camera panel
	if (Panels[1]) {

		ImGui::Begin(igTex[5]);

		//Vsync
		//ImGui::Checkbox(igSettings[1], &doVsync);
		//FOV
		ImGui::SliderFloat(igSettings[2], &cameraSettins[0], 0.1f, 160.0f);
		ImGui::DragFloat2(igSettings[7], &cameraSettins[1], cameraSettins[2]);

		//reset camera pos
		ImGui::Text(igTex[2]);
		if (ImGui::SmallButton(igSettings[3])) {
			TempButton = 1;
		}
		//set cam pos
		ImGui::DragFloat3(igSettings[6], CameraXYZ);
		if (ImGui::SmallButton("Set")) {
			TempButton = 2;
		}
		ImGui::End();
	}
	//Lighting panel
	if (Panels[2]) {
		ImGui::Begin(igTex[3]);

		switch (ShaderNum) {
		case 0: //us
			ImGui::Text("UnShaded");
			ImGui::Text(igTex[3]);
			//sky
			ImGui::ColorEdit4(igSettings[4], skyRGBA);
			//doReflections
			ImGui::SliderInt("doReflections", &doReflections, 0, 1);

			break;
		case 1: //spot
			ImGui::Text("Spot Light");
			ImGui::DragFloat3("Light Transform", LightTransform1);
			ImGui::Text(igTex[3]);
			ImGui::Text(igTex[4]);
			//sky and light
			ImGui::ColorEdit4(igSettings[4], skyRGBA);
			ImGui::ColorEdit4(igSettings[5], lightRGBA);
			ImGui::DragFloat("light I", &ConeSI[2]);
			ImGui::Text("cone size");
			ImGui::SliderFloat("cone Size (D: 0.95)", &ConeSI[1], 0.0f, 1.0f);
			ImGui::SliderFloat("cone Strength (D: 0.05)", &ConeSI[0], 0.0f, 0.90f);
			ImGui::Text("Light Angle");
			ImGui::DragFloat3("Cone Angle", ConeRot);
			break;
		case 2: //dir light
			ImGui::Text("Direct Light");
			ImGui::Text(igTex[3]);
			ImGui::Text(igTex[4]);
			//sky and light
			ImGui::ColorEdit4(igSettings[4], skyRGBA);
			ImGui::DragFloat("light I", &ConeSI[2]);
			ImGui::Text("Light Angle");
			ImGui::DragFloat3("Cone Angle", ConeRot);
			break;
		case 3:
			ImGui::Text("Point Light (BROKE)");
			ImGui::DragFloat3("Light Transform", LightTransform1);
			ImGui::Text(igTex[3]);
			ImGui::Text(igTex[4]);
			//sky and light
			ImGui::ColorEdit4(igSettings[4], skyRGBA);
			ImGui::ColorEdit4(igSettings[5], lightRGBA);
			ImGui::DragFloat("light I", &ConeSI[2]);
			ImGui::Text("cone size");
			ImGui::SliderFloat("cone Size (D: 0.95)", &ConeSI[1], 0.0f, 1.0f);
			ImGui::SliderFloat("cone Strength (D: 0.05)", &ConeSI[0], 0.0f, 0.90f);
			ImGui::Text("Light Angle");
			ImGui::DragFloat3("Cone Angle", ConeRot);
			break;
		}
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
	GLFWwindow* window = glfwCreateWindow(screenArea[0], screenArea[1], "Farquhar Engine OPEN GL - 1.3", NULL, NULL);

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

	//create a shader program and feed it shader and vertex files
	Shader shaderProgram("Shaders/Default.vert", "Shaders/Default.frag");

	shaderProgram.Activate();

	// Initialize ImGUI
	initializeImGui(window);
	//depth pass. render things in correct order. eg sky behind wall, dirt under water, not random order
	glEnable(GL_DEPTH_TEST);

	//need to do more research into this one
	glEnable(GL_CULL_FACE);

	//INITIALIZE CAMERA
	// camera ratio and pos
	Camera camera(screenArea[0], screenArea[1], glm::vec3(0.0f, 0.0f, 50.0f));

	//load the model //modelS
	//Model model("Assets/Models/test2/scene.glb");

	//texture loading problems


	Model model("Assets/Models/grass3/scene.gltf");
	Model model2("Assets/Models/wall/scene.gltf");
	Model model3("Assets/Models/harvy/scene.gltf");
	Model model4("Assets/Models/us/scene.gltf");
	//Model model3("Assets/Models/harvy/scene.gltf");
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
			framerate = "FPS " + std::to_string(frameRateI);
			timeAccumulator[0] = 0.0f;
		}

		timeAccumulator[1] += deltaTime;
		//60hz
		if (timeAccumulator[1] >= 0.016f) {
			//run if after .16 second
			if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_5) == GLFW_PRESS)
			{
				cameraSettins[0] += 0.2f;
			}
			if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_4) == GLFW_PRESS)
			{
				cameraSettins[0] -= 0.2f;
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

		//camera fov, near and far plane
		camera.updateMatrix(cameraSettins[0], cameraSettins[1], cameraSettins[2]);

		//draws the model to the screen
		model.Draw(shaderProgram, camera);
		model2.Draw(shaderProgram, camera);
		model3.Draw(shaderProgram, camera);
		model4.Draw(shaderProgram, camera);

		//2025 REWORK THESE PLEASE
		//i added these
		//transform light but not model
		glm::int8 ShaderNumber = glm::int8(ShaderNum);
		glm::vec4 lightColor = glm::vec4(lightRGBA[0], lightRGBA[1], lightRGBA[2], lightRGBA[3]);
		glm::vec3 InnerLight = glm::vec3(ConeSI[1] - ConeSI[0], ConeSI[1], ConeSI[2]);
		glm::vec3 spotLightRot = glm::vec3(ConeRot[0], ConeRot[1], ConeRot[2]);
		glm::vec4 skyColor = glm::vec4(skyRGBA[0], skyRGBA[1], skyRGBA[2], skyRGBA[3]);
		glm::int8 doReflect = glm::int8(doReflections);

		glm::vec3 lightPos = glm::vec3(LightTransform1[0], LightTransform1[1], LightTransform1[2]);
		glm::mat4 lightModel = glm::mat4(1.0f);
		lightModel = glm::translate(lightModel, lightPos);

		//shaderprog can stay
		//activate shader program
		shaderProgram.Activate();
		//i wrote 
		glUniform1i(glGetUniformLocation(shaderProgram.ID, "ShaderNumber"), ShaderNumber);
		glUniform1i(glGetUniformLocation(shaderProgram.ID, "doReflect"), doReflect);
		glUniform3f(glGetUniformLocation(shaderProgram.ID, "InnerLight1"), InnerLight.x, InnerLight.y, InnerLight.z);
		glUniform3f(glGetUniformLocation(shaderProgram.ID, "spotLightRot"), spotLightRot.x, spotLightRot.y, spotLightRot.z);
		glUniform4f(glGetUniformLocation(shaderProgram.ID, "skyColor"), skyColor.x, skyColor.y, skyColor.z, skyColor.w);

		//glUniform3f(glGetUniformLocation(shaderProgram.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
		//update light color seprate from the model
		glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
		// update light pos
		glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

		camera.Matrix(shaderProgram, "camMatrix");

		// Render ImGUI elements
		imGuiMAIN(window);

		switch (TempButton) {
		case 1:
			camera.Position = glm::vec3(0, 0, 0);
			TempButton = 0;
			break;
		case 2:
			camera.Position = glm::vec3(CameraXYZ[0], CameraXYZ[1], CameraXYZ[2]);
			TempButton = 0;
			break;
		}

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