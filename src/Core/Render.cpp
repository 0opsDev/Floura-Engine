#include "Render.h"
#include <Render/Cube/CubeVisualizer.h>
#include "Gameplay/BillboardObject.h"
#include "Gameplay/ModelObject.h"
#include <Sound/SoundProgram.h>
#include <utils/VisibilityChecker.h>
#include <glm/ext/vector_float3.hpp>
#include <glm/gtx/norm.hpp>
#include "Render/Cube/RenderQuad.h"
#include "scene.h"

Shader gPassShader;
Shader RenderClass::shaderProgram;
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

BillBoard LightIcon;
Shader SolidColour;
RenderQuad flatplanez;
Shader shader;
bool RenderClass::LightingPass = false; // Toggle for lighting pass
bool RenderClass::RegularPass = true; // Toggle for regular pass

void RenderClass::init(GLFWwindow* window, unsigned int width, unsigned int height) {

	ScreenUtils::setVSync(ScreenUtils::doVsync); // Set Vsync to value of doVsync (bool)

	// glenables
	// depth pass. render things in correct order. eg sky behind wall, dirt under water, not random order
	init::initGLenable(false); //bool for direction of polys
	gPassShader.LoadShader("Shaders/gBuffer/geometryPass.vert", "Shaders/gBuffer/geometryPass.frag");

	LightIcon.init("Assets/Dependants/LB.png");
	Skybox::init(Skybox::DefaultSkyboxPath);

	flatplanez.init();

	SolidColour.LoadShader("Shaders/Lighting/Default.vert", "Shaders/Db/solidColour.frag");

	shader.LoadShader("Shaders/Db/RenderQuad.vert", "Shaders/Db/RenderQuad.frag");

	// put in one function
	Framebuffer::setupMainFBO(width, height);
	Framebuffer::setupSecondFBO(width, height);
	Framebuffer::setupGbuffers(width, height);
	// need to add debug buffers at some point
	//Framebuffer::setupNoiseMap();

	init::initImGui(window); // Initialize ImGUI
}

void RenderClass::ClearFramebuffers() {

	// Clear first framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::FBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // Clear with colour
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Clear second framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::FBO2);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // Clear with colour
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Clear GBuffer
	glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::gBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear with colour
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderClass::Render(GLFWwindow* window, Shader frameBufferProgram, float window_width, float window_height, glm::vec3 lightPos,
	std::vector<std::tuple<Model, int, glm::vec3, glm::vec4, glm::vec3, int>> models) {

	// Camera
	Camera::Inputs(window); // send Camera.cpp window inputs and delta time
	glClearColor(RenderClass::skyRGBA[0], RenderClass::skyRGBA[1], RenderClass::skyRGBA[2],RenderClass::skyRGBA[3]);
	//AlbedoShader.Activate();
	//UF::Float(AlbedoShader.ID, "gamma", RenderClass::gamma);

	auto startInitTime = std::chrono::high_resolution_clock::now();
	// Bind the framebuffer for the geometry pass// g pass draw was here still some logic below

	//glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LESS);
	//glDisable(GL_CULL_FACE);
	for (auto& modelTuple : models) {
		Model& model = std::get<0>(modelTuple);
		int cullingSetting = std::get<1>(modelTuple);
		glm::vec3 translation = std::get<2>(modelTuple);
		glm::vec4 rotation = std::get<3>(modelTuple);
		glm::vec3 scale = std::get<4>(modelTuple);
		// Apply culling settings
		if (cullingSetting == 1 && !ImGuiCamera::isWireframe) { glEnable(GL_CULL_FACE); }
		else { glDisable(GL_CULL_FACE); }

		gPassDraw(model, translation, rotation, scale);
	}
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//FrameBuffer

	auto stopInitTime = std::chrono::high_resolution_clock::now();
	auto initDuration = std::chrono::duration_cast<std::chrono::microseconds>(stopInitTime - startInitTime);
	ImGuiCamera::gPassTime = (initDuration.count() / 1000.0);
	glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::FBO);
	glEnable(GL_DEPTH_TEST); // this line here caused me so much hell

	// Send Variables to shader (GPU)
	shaderProgram.Activate(); // activate shaderprog to send uniforms to gpu

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, Framebuffer::gPosition);
	shaderProgram.setInt("gPosition", 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, Framebuffer::gNormal);
	shaderProgram.setInt("gNormal", 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, Framebuffer::gAlbedoSpec);
	shaderProgram.setInt("gAlbedoSpec", 3);	

	shaderProgram.setBool("doReflect", doReflections);
	shaderProgram.setBool("doFog", doFog);
	shaderProgram.setFloat3("InnerLight1", ConeSI[1] - ConeSI[0], ConeSI[1], ConeSI[2]);
	shaderProgram.setFloat3("spotLightRot", ConeRot[0], ConeRot[1], ConeRot[2]);
	shaderProgram.setFloat3("lightPos", lightPos.x, lightPos.y, lightPos.z);
	shaderProgram.setFloat("DepthDistance", DepthDistance);
	shaderProgram.setFloat("NearPlane", DepthPlane[0]);
	shaderProgram.setFloat("FarPlane", DepthPlane[1]);
	shaderProgram.setFloat3("fogColor", fogRGBA[0], fogRGBA[1], fogRGBA[2]);
	shaderProgram.setFloat4("skyColor", skyRGBA[0], skyRGBA[1], skyRGBA[2], skyRGBA[3]);
	shaderProgram.setFloat4("lightColor", lightRGBA[0], lightRGBA[1], lightRGBA[2], lightRGBA[3]);
	shaderProgram.setFloat("gamma", gamma);
	if (ImGuiCamera::isWireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Enable wireframe mode
		glClearColor(0, 0, 0, 1), glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}
	auto startInitTime2 = std::chrono::high_resolution_clock::now();
	if (RegularPass) {
		
		for (auto& modelTuple : models) {
			Model& model = std::get<0>(modelTuple);
			int cullingSetting = std::get<1>(modelTuple);
			glm::vec3 translation = std::get<2>(modelTuple);
			glm::vec4 rotation = std::get<3>(modelTuple);
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
				SolidColour.setFloat("DepthDistance", 50);
				SolidColour.setFloat("NearPlane", RenderClass::DepthPlane[0]);
				SolidColour.setFloat("FarPlane", RenderClass::DepthPlane[1]);

				model.Draw(SolidColour, translation, rotation, scale);
				glDisable(GL_CULL_FACE);
			}
		}
	}

	//test2.rotation.x += 300 * TimeUtil::s_DeltaTime;
	//if (test2.rotation.x >= 360) { test2.rotation.x = 0; } // Reset rotation to prevent overflow

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Restore normal rendering < wireframe
	// Camera
	Camera::Matrix(shaderProgram, "camMatrix"); // Send Camera Matrix To Shader Prog


	LightIcon.draw(true, lightPos.x, lightPos.y, lightPos.z, 0.3, 0.3, 0.3);

	if (!ImGuiCamera::isWireframe) {
		Skybox::draw(RenderClass::skyRGBA, Camera::width, Camera::height); // cleanup later, put camera width and height inside skybox class since, they're already global
		glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::FBO);

	}
	//glEnable(GL_CULL_FACE);
	if (LightingPass) {
		//glDisable(GL_DEPTH_TEST);
		//glDepthFunc(GL_LESS);
		glDisable(GL_CULL_FACE);
		shader.Activate();
		// gPass textures bound to FB
		// send gPass textures to shader
		glActiveTexture(0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Framebuffer::gPosition);
		shader.setInt("gPosition", 1);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, Framebuffer::gNormal);
		shader.setInt("gNormal", 2);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, Framebuffer::gAlbedoSpec);
		shader.setInt("gAlbedoSpec", 3);

		shader.setFloat3("lightColor", lightRGBA[0], lightRGBA[1], lightRGBA[2]);
		shader.setFloat3("skyColor", skyRGBA[0], skyRGBA[1], skyRGBA[2]);
		shader.setFloat3("orientation", Camera::Orientation.x, Camera::Orientation.y, Camera::Orientation.z);
		shader.setFloat3("cameraPos", Camera::Position.x, Camera::Position.y, Camera::Position.z);
		//shader.
		flatplanez.draw(shader);
	}
	auto stopInitTime2 = std::chrono::high_resolution_clock::now();
	auto initDuration2 = std::chrono::duration_cast<std::chrono::microseconds>(stopInitTime2 - startInitTime2);
	ImGuiCamera::lPassTime = (initDuration2.count() / 1000.0);

	//glDepthFunc(GL_LEQUAL);
	glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::FBO);
	//glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_CULL_FACE);

	// Framebuffer logic
	Framebuffer::FBODraw(frameBufferProgram, ImGuiCamera::imGuiPanels[0], window_width, window_height, window);
}

void RenderClass::Swapchain(GLFWwindow* window, Shader frameBufferProgram, GLFWmonitor* primaryMonitor) {


	Camera::updateMatrix(Main::cameraSettings[0], Main::cameraSettings[1], Main::cameraSettings[2]); // Update: fov, near and far plane

	glfwSwapBuffers(window); // Swap BackBuffer with FrontBuffer (DoubleBuffering)
	glfwPollEvents(); // Tells open gl to proccess all events such as window resizing, inputs (KBM)
}

void RenderClass::Cleanup() {
	shaderProgram.Delete(); // Delete Shader Prog
}

void RenderClass::gPassDraw(Model& model, glm::vec3 Transform, glm::vec4 Rotation, glm::vec3 Scale) {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	gPassShader.Activate();
	gPassShader.setFloat("gamma", RenderClass::gamma);
	glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::gBuffer);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	Camera::Matrix(gPassShader, "camMatrix"); // Send Camera Matrix To Shader Prog
	model.Draw(gPassShader, Transform, Rotation, Scale);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//FrameBuffer
	glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::FBO);
}
