#include "Render.h"
#include <Render/Cube/CubeVisualizer.h>
#include <Render/Cube/Billboard.h>

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
glm::vec3 RenderClass::CameraXYZ = glm::vec3( 0.0f, 0.0f, 0.0f ); // Initial camera position
CubeVisualizer cv;
BillBoard  bv1;
void RenderClass::init(GLFWwindow* window, unsigned int width, unsigned int height) {

	ScreenUtils::setVSync(ScreenUtils::doVsync); // Set Vsync to value of doVsync (bool)

	// glenables
	// depth pass. render things in correct order. eg sky behind wall, dirt under water, not random order
	init::initGLenable(false); //bool for direction of polys

	cv.init();
	bv1.init("Assets/Sprites/pot.png");
	Skybox::init(Skybox::DefaultSkyboxPath);

	// put in one function
	Framebuffer::setupMainFBO(width, height);
	Framebuffer::setupSecondFBO(width, height);

	init::initImGui(window); // Initialize ImGUI
}

void RenderClass::Render(GLFWwindow* window, Camera& camera, Shader frameBufferProgram, Shader shaderProgram, Shader LightProgram, Shader SolidColour, float window_width, float window_height, glm::vec3 lightPos, Model Lightmodel,
	std::vector<std::tuple<Model, int, glm::vec3, glm::quat, glm::vec3, int>> models) {

	//FrameBuffer
	glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::FBO);
	// Clear BackBuffer
	glClearColor(RenderClass::skyRGBA[0], RenderClass::skyRGBA[1], RenderClass::skyRGBA[2], RenderClass::skyRGBA[3]), glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // Clear with colour
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
			model.Draw(shaderProgram, camera, translation, rotation, scale);
			glDisable(GL_CULL_FACE);
			Lightmodel.Draw(LightProgram, camera, lightPos, glm::quat(0, 0, 0, 0), glm::vec3(0.3f, 0.3f, 0.3f));
		}
		else {
			SolidColour.Activate();
			UF::Depth(SolidColour.ID, 50, RenderClass::DepthPlane);

			model.Draw(SolidColour, camera, translation, rotation, scale);
			glDisable(GL_CULL_FACE);
			Lightmodel.Draw(SolidColour, camera, lightPos, glm::quat(0, 0, 0, 0), glm::vec3(0.3f, 0.3f, 0.3f));
		}
	}

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Restore normal rendering < wireframe
	// Camera
	camera.Matrix(shaderProgram, "camMatrix"); // Send Camera Matrix To Shader Prog
	camera.Matrix(LightProgram, "camMatrix"); // Send Camera Matrix To Shader Prog
	bv1.draw(camera, false, 0, 0.45, 3, 0.5, 0.5, 0.5);

	cv.draw(camera, 0,0,0,1, 2, 1); // is rendering , depth is just messing with it

	if (!ImGuiCamera::isWireframe) {
		Skybox::draw(camera, RenderClass::skyRGBA, camera.width, camera.height); // cleanup later, put camera width and height inside skybox class since, they're already global
	}

	// Framebuffer logic
	Framebuffer::FBODraw(frameBufferProgram, ImGuiCamera::imGuiPanels[0], window_width, window_height, window, camera);
}

void RenderClass::Swapchain(GLFWwindow* window, Camera& camera, Shader frameBufferProgram, Shader shaderProgram, GLFWmonitor* primaryMonitor) {

	// Camera
	camera.Inputs(window); // send Camera.cpp window inputs and delta time
	camera.updateMatrix(Main::cameraSettings[0], Main::cameraSettings[1], Main::cameraSettings[2]); // Update: fov, near and far plane

	glfwSwapBuffers(window); // Swap BackBuffer with FrontBuffer (DoubleBuffering)
	glfwPollEvents(); // Tells open gl to proccess all events such as window resizing, inputs (KBM)
}

void RenderClass::Cleanup() {

}