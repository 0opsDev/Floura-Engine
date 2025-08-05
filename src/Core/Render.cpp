#include "Render.h"
#include <Render/Cube/CubeVisualizer.h>
#include "Gameplay/BillboardObject.h"
#include "Gameplay/ModelObject.h"
#include <Sound/SoundProgram.h>
#include <glm/ext/vector_float3.hpp>
#include <glm/gtx/norm.hpp>
#include "Render/Cube/RenderQuad.h"
#include "tempscene.h"
#include <Render/passes/geometry/geometryPass.h>
#include <Render/passes/lighting/LightingPass.h>
#include <Render/window/WindowHandler.h>

Shader RenderClass::billBoardShader;
Shader RenderClass::gPassShaderBillBoard;
Shader RenderClass::boxShader;

float RenderClass::gamma = 2.2f;
bool RenderClass::renderSkybox = true;
bool RenderClass::doReflections = true;
bool RenderClass::doFog = true;
GLfloat RenderClass::DepthDistance = 100.0f;
GLfloat RenderClass::DepthPlane[] = { 0.1f, 100.0f };
glm::vec3 RenderClass::skyRGBA = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 RenderClass::fogRGBA = glm::vec3( 1.0f, 1.0f, 1.0f);

Shader SolidColour;
RenderQuad lightingRenderQuad;
Shader GBLpass;

bool RenderClass::DoDeferredLightingPass = false; // Toggle for lighting pass
bool RenderClass::DoForwardLightingPass = true; // Toggle for regular pass
bool RenderClass::DoComputeLightingPass = false;

void RenderClass::init(unsigned int width, unsigned int height) {

	windowHandler::setVSync(windowHandler::doVsync); // Set Vsync to value of doVsync (bool)

	// glenables
	// depth pass. render things in correct order. eg sky behind wall, dirt under water, not random order
	init::initGLenable(false); //bool for direction of polys
	GeometryPass::init(); // Initialize geometry pass settings

	lightingRenderQuad.init();

	
	// put in one function
	Framebuffer::setupMainFBO(width, height);
	Framebuffer::setupSecondFBO(width, height);
	GeometryPass::setupGbuffers(width, height); // here
	LightingPass::initcomputeShader(width, height); // Initialize compute shader for lighting pass

	initGlobalShaders();
	// need to add debug buffers at some point
	//Framebuffer::setupNoiseMap();

	if (FEImGuiWindow::imGuiEnabled) {
		FEImGuiWindow::init();
		ImGuizmo::SetOrthographic(false);
		init::initImGui(windowHandler::window); // Initialize ImGUI
	}
	
}

void RenderClass::initGlobalShaders() {
	// cube collider and billboard, oh yeah and framebuffer
	billBoardShader.LoadShader("Shaders/Db/BillBoard.vert", "Shaders/Db/BillBoard.frag");
	gPassShaderBillBoard.LoadShader("Shaders/gBuffer/geometryPassBillboard.vert", "Shaders/gBuffer/geometryPassBillboard.frag");
	boxShader.LoadShader("Shaders/Lighting/Default.vert", "Shaders/Db/OrangeHitbox.frag");
	SolidColour.LoadShader("Shaders/Lighting/Default.vert", "Shaders/Db/solidColour.frag");
	GBLpass.LoadShader("Shaders/Db/RenderQuad.vert", "Shaders/Db/RenderQuad.frag");
	Framebuffer::frameBufferProgram.LoadShader("Shaders/PostProcess/framebuffer.vert", "Shaders/PostProcess/framebuffer.frag");
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
	glBindFramebuffer(GL_FRAMEBUFFER, GeometryPass::gBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear with colour
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderClass::Render(GLFWwindow* window, unsigned int width, unsigned int height) {

	glClearColor(RenderClass::skyRGBA[0], RenderClass::skyRGBA[1], RenderClass::skyRGBA[2],RenderClass::skyRGBA[3]);
	//AlbedoShader.Activate();
	//UF::Float(AlbedoShader.ID, "gamma", RenderClass::gamma);

	auto startInitTime = std::chrono::high_resolution_clock::now();
	// Bind the framebuffer for the geometry pass// g pass draw was here still some logic below

	//glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LESS);
	//glDisable(GL_CULL_FACE);
	
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//FrameBuffer

	auto stopInitTime = std::chrono::high_resolution_clock::now();
	auto initDuration = std::chrono::duration_cast<std::chrono::microseconds>(stopInitTime - startInitTime);
	glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::FBO);
	glEnable(GL_DEPTH_TEST); // this line here caused me so much hell

	/*
	shaderProgram.setBool("doReflect", doReflections);
	shaderProgram.setBool("doFog", doFog);
	shaderProgram.setFloat3("InnerLight1", ConeSI[1] - ConeSI[0], ConeSI[1], ConeSI[2]);
	shaderProgram.setFloat3("spotLightRot", ConeRot[0], ConeRot[1], ConeRot[2]);
	shaderProgram.setFloat("DepthDistance", DepthDistance);
	shaderProgram.setFloat("NearPlane", DepthPlane[0]);
	shaderProgram.setFloat("FarPlane", DepthPlane[1]);
	shaderProgram.setFloat3("fogColor", fogRGBA.r, fogRGBA.g, fogRGBA.b);
	shaderProgram.setFloat4("skyColor", skyRGBA.r, skyRGBA.g, skyRGBA.b, skyRGBA[3]);
	shaderProgram.setFloat("gamma", gamma);
	shaderProgram.setFloat("deltatime", TimeUtil::s_DeltaTime);
	shaderProgram.setFloat("time", glfwGetTime());
	*/

	if (FEImGuiWindow::isWireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Enable wireframe mode
		glClearColor(0, 0, 0, 1);
	}
	auto startInitTime2 = std::chrono::high_resolution_clock::now();
	//if (DoForwardLightingPass) {	
	//}

	//test2.rotation.x += 300 * TimeUtil::s_DeltaTime;
	//if (test2.rotation.x >= 360) { test2.rotation.x = 0; } // Reset rotation to prevent overflow

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Restore normal rendering < wireframe


	if (!FEImGuiWindow::isWireframe && RenderClass::renderSkybox) { // should add skybox.scene
		Skybox::draw(Camera::width, Camera::height); // cleanup later, put camera width and height inside skybox class since, they're already global
		glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::FBO);

	}
	if (DoDeferredLightingPass) {
		DeferredLightingPass(); // Forward Lighting Pass
	}
	if (DoComputeLightingPass) {
		LightingPass::computeRender(); // Run compute shader for lighting pass
	}

	auto stopInitTime2 = std::chrono::high_resolution_clock::now();
	auto initDuration2 = std::chrono::duration_cast<std::chrono::microseconds>(stopInitTime2 - startInitTime2);
	FEImGuiWindow::lPassTime = (initDuration2.count() / 1000.0);

	//glDepthFunc(GL_LEQUAL);
	glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::FBO);
	//glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_CULL_FACE);

	// Framebuffer logic
	Framebuffer::FBODraw(FEImGuiWindow::imGuiPanels[0], window);
}

void RenderClass::ForwardLightingPass() {
	
}

void RenderClass::DeferredLightingPass() {

	glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::FBO);
	//glEnable(GL_CULL_FACE);
		//glDisable(GL_DEPTH_TEST);
		//glDepthFunc(GL_LESS);
	glDisable(GL_CULL_FACE);
	GBLpass.Activate();
	// gPass textures bound to FB
	// send gPass textures to shader
	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	GBLpass.setBool("DEFtoggle", FEImGuiWindow::enableDEF);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, GeometryPass::gPosition);
	GBLpass.setInt("gPosition", 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, GeometryPass::gNormal);
	GBLpass.setInt("gNormal", 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, GeometryPass::gAlbedoSpec);
	GBLpass.setInt("gAlbedoSpec", 3);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, GeometryPass::depthTexture);
	GBLpass.setInt("depthMap", 5);

	GBLpass.setFloat("DepthDistance", DepthDistance);
	GBLpass.setFloat("NearPlane", DepthPlane[0]);
	GBLpass.setFloat("FarPlane", DepthPlane[1]);
	GBLpass.setBool("doFog", doFog);
	GBLpass.setFloat3("fogColor", fogRGBA.r, fogRGBA.g, fogRGBA.b);

	//mat4
	GBLpass.setMat4("cameraMatrix", Camera::cameraMatrix);
	GBLpass.setMat4("projectionMatrix", Camera::projection);

	glm::mat4 inverseProjection = glm::inverse(Camera::projection);
	GBLpass.setMat4("inverseProjection", inverseProjection);

	glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(Camera::cameraMatrix)));
	GBLpass.setMat3("normalMatrix", normalMatrix);

	GBLpass.setFloat3("orientation", Camera::Orientation.x, Camera::Orientation.y, Camera::Orientation.z);
	GBLpass.setFloat3("cameraPos", Camera::Position.x, Camera::Position.y, Camera::Position.z);
	GBLpass.setFloat3("cameraDirection", Camera::Orientation.x, Camera::Orientation.y, Camera::Orientation.z);
	//std::cout << Camera::width << " " << Camera::height << std::endl;
	GBLpass.setFloat2("screenSize", Camera::width, Camera::height);
	GBLpass.setFloat("time", glfwGetTime());
	//shader.
	lightingRenderQuad.draw(GBLpass);

	glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::FBO);

}

void RenderClass::HybridLightingPass() {

}

void RenderClass::Swapchain(GLFWwindow* window) {


	Camera::updateMatrix(Main::cameraSettings[0], Main::cameraSettings[1], Main::cameraSettings[2]); // Update: fov, near and far plane

	glfwSwapBuffers(window); // Swap BackBuffer with FrontBuffer (DoubleBuffering)
	glfwPollEvents(); // Tells open gl to proccess all events such as window resizing, inputs (KBM)
}

void RenderClass::Cleanup() {
	billBoardShader.Delete();
	gPassShaderBillBoard.Delete();
	boxShader.Delete();
	GBLpass.Delete();
	SolidColour.Delete();
	billBoardShader.Delete();
	Framebuffer::frameBufferProgram.Delete();
}