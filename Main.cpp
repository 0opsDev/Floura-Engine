#include"src/video/Model.h"
#include"imgui.h"
#include <fstream>
#include <stdexcept>
#include <iostream>
#include"imgui_impl_glfw.h"
#include"imgui_impl_opengl3.h"
// these two where const

unsigned int screenArea[2] = { 2560, 1440  };
int screenAreaI[2] = { screenArea[0], screenArea[1] };
//https://discord.gg/fd6REHgBus

// Initialize previous time and delta time
float lastFrameTime = 0.0f;
float deltaTime = 0.0f;

GLfloat varFOV = 60.0f;
GLfloat lightRGBA[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat skyRGBA[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat CameraXYZ[3] = { 0.0f, 0.0f, 50.0f };
bool doVsync = false;
const char* igSettings[] =
{
	"Settings Window" , "Vsync", "FOV",
	"Reset Camera",
	"sky RGBA", "light RGBA", "Camera Transform"
};
const char* igTex[] =
{
	"Settings (Press escape to use mouse)" , "Rendering",
	"Transform", "Lighting", "Light color and intens", 
};

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
			case 5:
				if (iss >> value) skyRGBA[0] = value;
				break;
			case 6:
				if (iss >> value) skyRGBA[1] = value;
				break;
			case 7:
				if (iss >> value) skyRGBA[2] = value;
				break;
			case 11:
				if (iss >> value) lightRGBA[0] = value;
				break;
			case 12:
				if (iss >> value) lightRGBA[1] = value;
				break;
			case 13:
				if (iss >> value) lightRGBA[2] = value;
				break;
			case 15:
				if (iss >> value) screenArea[0] = value;
				break;
			case 16:
				if (iss >> value) screenArea[1] = value;
				break;
			case 1:
				doVsync = (lineT == "VsyncT");
				std::cout << doVsync << " Vsync" << std::endl;
				break;
			default:
				break;
			}
		}
		TestFile2.close();
	}
};

int main()
{
	//calls the LoadSettings function
	loadSettings();
	//initialize inside of function
	//start glfw
	glfwInit();

	//give glfw hints on open gl version (3.3)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, 1);



	//size, name, fullscreen
	GLFWwindow* window = glfwCreateWindow(screenArea[0], screenArea[1], "Farquhar Engine OPEN GL - 1.1a", NULL, NULL);
	

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
	glViewport(0, 0, screenArea[0], screenArea[1]);


	//create a shader program and feed it shader and vertex files
	Shader shaderProgram("Shaders/Default.vert", "Shaders/Default.frag");

	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);

	shaderProgram.Activate();
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	// Initialize ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	//imgui vars
	bool save = false;
	float rotationStored = 50.0f;
	float rotation = 0.0f;
	bool drawTriangles = true;
	bool ResetTrans = false;
	GLfloat ConeSI[3] = { 0.05f, 0.95f , 1.0f };
	GLfloat ConeRot[3] = { 0.0f, -1.0f , 0.0f };
	GLfloat LightTransform1[3] = { -2.0f, 5.0f, 0.0f };


	GLfloat value = 0.0f;


	//depth pass. render things in correct order. eg sky behind wall, dirt under water, not random order
	glEnable(GL_DEPTH_TEST);

	//need to do more research into this one
	//glEnable(GL_CULL_FACE);

	//INITIALIZE CAMERA
	// camera ratio and pos
	Camera camera(screenArea[0], screenArea[1], glm::vec3(0.0f, 0.0f, 50.0f));

	Model model("Assets/Models/Sword/scene.gltf");

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
	switch (doVsync) {
	case true:
		//glfwSwapInterval(1);
		glfwSwapInterval(1);
		break;
	case false:
		glfwSwapInterval(0);
		break;
	}


	//game loop
	//makes sure window stays open
	while (!glfwWindowShouldClose(window))
	{



		// Calculate delta time
		// Cast the value to float

		float currentFrameTime = static_cast<float>(glfwGetTime());
		deltaTime = currentFrameTime - lastFrameTime;
		lastFrameTime = currentFrameTime;


		//array of settings files which determines what line to read on the ini fale




		//if (save) {
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

		//	save = false;
		//}

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_5) == GLFW_PRESS)
		{
			varFOV += 0.2f;
		}
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_4) == GLFW_PRESS)
		{
			varFOV -= 0.2f;
		}
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
		{
			varFOV = 60.0f;
		}
		if (varFOV <= 0.00f)
		{
			varFOV = 0.1f;
		}
		if (varFOV >= 160.1f)
		{
			varFOV = 160.0f;
		}

		//Tell OpenGL a new frame is about to begin
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		//CLEAR BACK BUFFER
		//RGB ALPHA please re enable to fix sky colour
		glClearColor(skyRGBA[0], skyRGBA[1], skyRGBA[2], skyRGBA[3]);
		// SEND TO COLOR BUFFER (DRAWS COLOUR TO SCREEN)
		//glclear only needs to run once btw
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//inputs
		camera.Inputs(window, deltaTime);

		//camera fov, near and far plane
		camera.updateMatrix(varFOV, 0.1f, 100.0f);

		model.Draw(shaderProgram, camera);



		//2025 REWORK THESE PLEASE
		//i added these
		//transform light but not model
		glm::vec4 lightColor = glm::vec4(lightRGBA[0], lightRGBA[1], lightRGBA[2], lightRGBA[3]);
		glm::vec3 InnerLight = glm::vec3(ConeSI[1] - ConeSI[0], ConeSI[1], ConeSI[2]);
		glm::vec3 spotLightRot = glm::vec3(ConeRot[0], ConeRot[1], ConeRot[2]);
		glm::vec4 skylightSpread = glm::vec4(skyRGBA[0], skyRGBA[1], skyRGBA[2], skyRGBA[3]);

		glm::vec3 lightPos = glm::vec3(LightTransform1[0], LightTransform1[1], LightTransform1[2]);
		glm::mat4 lightModel = glm::mat4(1.0f);
		lightModel = glm::translate(lightModel, lightPos);

		//update light transform
		//glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(lightModel));
		//update light color on the model (not the world)
		//glUniform4f(glGetUniformLocation(lightShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);

		//shaderprog can stay
		//activate shader program
		shaderProgram.Activate();
		//i wrote innerlight
		glUniform3f(glGetUniformLocation(shaderProgram.ID, "InnerLight1"), InnerLight.x, InnerLight.y, InnerLight.z);
		glUniform3f(glGetUniformLocation(shaderProgram.ID, "spotLightRot"), spotLightRot.x, spotLightRot.y, spotLightRot.z);
		glUniform4f(glGetUniformLocation(shaderProgram.ID, "skylightSpread"), skylightSpread.x, skylightSpread.y, skylightSpread.z, skylightSpread.w);

		glUniform3f(glGetUniformLocation(shaderProgram.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
		//update light color seprate from the model
		glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
		// update light pos
		glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
		//


		camera.Matrix(shaderProgram, "camMatrix");

		// ImGUI window creation
		ImGui::Begin(igSettings[0]);

		ImGui::Text(igTex[0]);
		//load settings button
		if (ImGui::SmallButton("load")) {
			loadSettings();
		}
		//ImGui::Checkbox("save changes?", &save);
		//rendering
		ImGui::Text(igTex[1]);
		//do vsync
		ImGui::Checkbox(igSettings[1], &doVsync);
		//screen res
		ImGui::DragInt("Width", &screenAreaI[0]);
		ImGui::DragInt("Height", &screenAreaI[1]);
		//apply button
		if (ImGui::SmallButton("Apply Changes?")) {

			screenArea[0] = screenAreaI[0];
			screenArea[1] = screenAreaI[1];
			glViewport(0, 0, screenArea[0], screenArea[1]);
			glfwSetWindowSize(window, screenArea[0], screenArea[1]);

			switch (doVsync) {
			case true:
				//glfwSwapInterval(1);
				glfwSwapInterval(1);
				break;
			case false:
				glfwSwapInterval(0);
				break;
			}
		}
		//Vsync
		//ImGui::Checkbox(igSettings[1], &doVsync);
		//FOV
		ImGui::SliderFloat(igSettings[2], &varFOV, 0.1f, 160.0f);

		//reset camera pos
		ImGui::Text(igTex[2]);
		if (ImGui::SmallButton(igSettings[3])) {
			camera.Position = glm::vec3(0, 0, 0);
		}
		//set cam pos
		ImGui::DragFloat3(igSettings[6], CameraXYZ);
		if (ImGui::SmallButton("Set")) {
			camera.Position = glm::vec3(CameraXYZ[0], CameraXYZ[1], CameraXYZ[2]);
		}

		//ImGui::DragFloat3("Light Yransform", LightTransform1);



		ImGui::Text(igTex[3]);
		ImGui::Text(igTex[4]);
		//sky and light
		ImGui::ColorEdit4(igSettings[4], skyRGBA);
		ImGui::ColorEdit4(igSettings[5], lightRGBA);
		//ImGui::DragFloat("light I", &ConeSI[2]);
		//ImGui::Text("cone size");
		//ImGui::SliderFloat("cone Size (D: 0.95)", &ConeSI[1], 0.0f, 1.0f);
		//ImGui::SliderFloat("cone Strength (D: 0.05)", &ConeSI[0], 0.0f, 0.90f);
		//ImGui::Text("Light Angle");
		//ImGui::DragFloat3("Cone Angle", ConeRot);

		// Ends the window
		ImGui::End();

		// Renders the ImGUI elements
		ImGui::Render();
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