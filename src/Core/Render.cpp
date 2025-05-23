#include "Render.h"
#include <Render/Cube/CubeVisualizer.h>
#include "Gameplay/BillboardObject.h"
#include <Render/Cube/Billboard.h>
#include "Gameplay/ModelObject.h"

float RenderClass::gamma = 2.2f;
bool RenderClass::doReflections = true;
bool RenderClass::doFog = true;
GLfloat RenderClass::DepthDistance = 100.0f;
GLfloat RenderClass::DepthPlane[] = { 0.1f, 100.0f };
GLfloat RenderClass::lightRGBA[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat RenderClass::skyRGBA[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat RenderClass::fogRGBA[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat RenderClass::LightTransform1[] = { 0.0f, 5.0f, 0.0f };
GLfloat RenderClass::ConeSI[3] = { 0.111f, 0.825f, 2.0f };
GLfloat RenderClass::ConeRot[3] = { 0.0f, -1.0f, 0.0f };
glm::vec3 RenderClass::CameraXYZ = glm::vec3(0.0f, 0.0f, 0.0f); // Initial camera position
BillBoardObject BBOJ2;
BillBoardObject BBOJ;
BillBoard LightIcon;
ModelObject test;
Shader AlbedoShader("skip", "");
void RenderClass::init(GLFWwindow* window, unsigned int width, unsigned int height) {

	ScreenUtils::setVSync(ScreenUtils::doVsync); // Set Vsync to value of doVsync (bool)

	// glenables
	// depth pass. render things in correct order. eg sky behind wall, dirt under water, not random order
	init::initGLenable(false); //bool for direction of polys

	BBOJ2.CreateObject("Animated", "Assets/Sprites/animatedBillboards/realistic fire/fire.json", "fire");
	BBOJ2.tickrate = 20;
	BBOJ2.doPitch = true;
	BBOJ2.transform = glm::vec3(5, 5, 5);
	BBOJ2.scale = glm::vec3(1, 1, 1);
	BBOJ2.isCollider = true;
	BBOJ.CreateObject("Static", "Assets/Sprites/pot.png", "pot");
	BBOJ.doPitch = false;
	BBOJ.transform = glm::vec3(0, 0.45, 3);
	BBOJ.scale = glm::vec3(0.5, 0.5, 0.5);
	BBOJ.isCollider = true;
	LightIcon.init("Assets/Dependants/LB.png");
	Skybox::init(Skybox::DefaultSkyboxPath);
	test.CreateObject("LOD", "Assets/LodModel/Vase/VaseLod.json", "test");
	test.transform = glm::vec3(5, 2, 0);
	test.isCollider = true;
	AlbedoShader = Shader("Shaders/gBuffer/geometryPass.vert", "Shaders/gBuffer/geometryPass.frag");
	// put in one function
	Framebuffer::setupMainFBO(width, height);
	Framebuffer::setupSecondFBO(width, height);
	Framebuffer::setupGbuffers(width, height);

	init::initImGui(window); // Initialize ImGUI
}

void RenderClass::Render(GLFWwindow* window, Shader frameBufferProgram, Shader shaderProgram, Shader LightProgram, Shader SolidColour, float window_width, float window_height, glm::vec3 lightPos,
	std::vector<std::tuple<Model, int, glm::vec3, glm::quat, glm::vec3, int>> models) {

	// Camera
	Camera::Inputs(window); // send Camera.cpp window inputs and delta time
	glClearColor(RenderClass::skyRGBA[0], RenderClass::skyRGBA[1], RenderClass::skyRGBA[2],RenderClass::skyRGBA[3]);
	AlbedoShader.Activate();
	UF::Float(AlbedoShader.ID, "gamma", RenderClass::gamma);
	glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::gBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear with colour
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDisable(GL_CULL_FACE);
	for (auto& modelTuple : models) {
		Model& model = std::get<0>(modelTuple);
		glm::vec3 translation = std::get<2>(modelTuple);
		glm::quat rotation = std::get<3>(modelTuple);
		glm::vec3 scale = std::get<4>(modelTuple);

			model.Draw(AlbedoShader, translation, rotation, scale);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//FrameBuffer
	glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::FBO);
	// Clear BackBuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // Clear with colour
	glEnable(GL_DEPTH_TEST); // this line here caused me so much hell

	// Send Variables to shader (GPU)
	shaderProgram.Activate(); // activate shaderprog to send uniforms to gpu
	UF::Bool(shaderProgram.ID, "doReflect", RenderClass::doReflections);
	UF::Bool(shaderProgram.ID, "doFog", RenderClass::doFog);
	UF::TrasformUniforms(shaderProgram.ID, RenderClass::ConeSI, RenderClass::ConeRot, glm::vec3(RenderClass::LightTransform1[0], RenderClass::LightTransform1[1], RenderClass::LightTransform1[2]));
	UF::Depth(shaderProgram.ID, RenderClass::DepthDistance, RenderClass::DepthPlane);
	UF::ColourUniforms(shaderProgram.ID, RenderClass::fogRGBA, RenderClass::skyRGBA, RenderClass::lightRGBA, RenderClass::gamma);

	LightProgram.Activate();
	UF::Float4(LightProgram.ID, "lightColor", RenderClass::lightRGBA[0], RenderClass::lightRGBA[1], RenderClass::lightRGBA[2], RenderClass::lightRGBA[3]);

	if (ImGuiCamera::isWireframe) {
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
		if (cullingSetting == 1 && !ImGuiCamera::isWireframe) { glEnable(GL_CULL_FACE); }
		else { glDisable(GL_CULL_FACE); }

		if (!ImGuiCamera::isWireframe) {
			model.Draw(shaderProgram, translation, rotation, scale);
			glDisable(GL_CULL_FACE);
		}
		else {
			SolidColour.Activate();
			UF::Depth(SolidColour.ID, 50, RenderClass::DepthPlane);

			model.Draw(SolidColour, translation, rotation, scale);
			glDisable(GL_CULL_FACE);
		}
	}
	test.UpdateCollider();
	test.UpdateCameraCollider();
	test.draw(shaderProgram);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Restore normal rendering < wireframe
	// Camera
	Camera::Matrix(shaderProgram, "camMatrix"); // Send Camera Matrix To Shader Prog
	Camera::Matrix(LightProgram, "camMatrix"); // Send Camera Matrix To Shader Prog
	BBOJ2.draw();
	BBOJ2.UpdateCollider();
	BBOJ2.UpdateCameraCollider();
	BBOJ.draw();
	BBOJ.UpdateCollider();
	BBOJ.UpdateCameraCollider();
	LightIcon.draw(true, lightPos.x, lightPos.y, lightPos.z, 0.3, 0.3, 0.3);

	glDisable(GL_CULL_FACE);

	if (!ImGuiCamera::isWireframe) {
		Skybox::draw(RenderClass::skyRGBA, Camera::width, Camera::height); // cleanup later, put camera width and height inside skybox class since, they're already global
	}

	// Framebuffer logic
	Framebuffer::FBODraw(frameBufferProgram, ImGuiCamera::imGuiPanels[0], window_width, window_height, window);
}

void RenderClass::Swapchain(GLFWwindow* window, Shader frameBufferProgram, Shader shaderProgram, GLFWmonitor* primaryMonitor) {


	Camera::updateMatrix(Main::cameraSettings[0], Main::cameraSettings[1], Main::cameraSettings[2]); // Update: fov, near and far plane

	glfwSwapBuffers(window); // Swap BackBuffer with FrontBuffer (DoubleBuffering)
	glfwPollEvents(); // Tells open gl to proccess all events such as window resizing, inputs (KBM)
}

void RenderClass::Cleanup() {

}