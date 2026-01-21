#include "Render.h"
#include <Render/Cube/CubeVisualizer.h>
#include <glm/ext/vector_float3.hpp>
#include <glm/gtx/norm.hpp>
#include "Render/Object/RenderQuad.h"
#include <Render/passes/geometry/geometryPass.h>
#include <Render/passes/lighting/raytracer.h>
#include <Render/window/WindowHandler.h>
#include <Scene/LightingHandler.h>
#include <Scene/scene.h>
#include "Render/passes/post/denoise.h"
#include <Render/Handler/RenderHandler.h>

Shader RenderClass::billBoardShader;
Shader RenderClass::gPassShaderBillBoard;
Shader RenderClass::boxShader;
Shader RenderClass::LineShader;

bool RenderClass::renderSkybox = true;
bool RenderClass::doReflections = true;
bool RenderClass::doFog = true;
GLfloat RenderClass::DepthDistance = 100.0f;
GLfloat RenderClass::DepthPlane[] = { 0.1f, 100.0f };
glm::vec3 RenderClass::skyRGBA = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 RenderClass::fogRGBA = glm::vec3( 1.0f, 1.0f, 1.0f);

CubeVisualizer* RenderClass::WhiteCube;
Line3D* RenderClass::line;
Texture* RenderClass::bluenoise;

Shader SolidColour;
RenderQuad lightingRenderQuad;
Shader GBLpass;

bool RenderClass::DoDeferredLightingPass = false; // Toggle for lighting pass
bool RenderClass::DoForwardLightingPass = true; // Toggle for regular pass
bool RenderClass::DoComputeLightingPass = false;

void initGLenable(bool frontFaceSide) {
	// glenables
	// depth pass. render things in correct order. eg sky behind wall, dirt under water, not random order
	glEnable(GL_DEPTH_TEST); // Depth buffer
	glDepthFunc(GL_LESS);
	//glEnable(GL_STENCIL_TEST); //stencil buffer
	//glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glEnable(GL_CULL_FACE); // Culling
	glCullFace(GL_BACK);

	switch (frontFaceSide) { //currently set to false
	case true: { glFrontFace(GL_CW); break; } // inside facing
	case false: { glFrontFace(GL_CCW); break; } // outside facing
	}

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glEnable(GL_FRAMEBUFFER_SRGB);
	//glEnable(GL_MULTISAMPLE);
}

void RenderClass::init(unsigned int width, unsigned int height) {

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4), glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6); // Window Minimum and Maximum version
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //OpenGl Profile
	glfwWindowHint(GLFW_RESIZABLE, 1); // Start Resizable
	glfwWindowHint(GLFW_MAXIMIZED, 0); // Start Maximized
	glfwWindowHint(GLFW_DEPTH_BITS, 24); // DepthBuffer Bit
	glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

	//
	//glfwWindowHint(GLFW_RED_BITS, 10);
	//glfwWindowHint(GLFW_GREEN_BITS, 10);
	//glfwWindowHint(GLFW_BLUE_BITS, 10);
	//glfwWindowHint(GLFW_ALPHA_BITS, 2);

	windowHandler::InitMainWidnow();
	if (!gladLoadGL(glfwGetProcAddress)) {
		// Log that GLAD failed to initialize
		std::cerr << "Failed to initialize GLAD" << std::endl;
		return;
	}



	windowHandler::setVSync(windowHandler::doVsync); // Set Vsync to value of doVsync (bool)

	// glenables
	// depth pass. render things in correct order. eg sky behind wall, dirt under water, not random order
	initGLenable(false); //bool for direction of polys

	GeometryPass::init(); // Initialize geometry pass settings

	lightingRenderQuad.init();
	// put in one function
	Framebuffer::setupFBO(width, height);
	GeometryPass::setupGbuffers(width, height); // here
	raytracer::init();

	// load bluenoise texture
	bluenoise = new Texture(); bluenoise->createTexture("Assets/Dependants/LDR_LLL1_0.png", "misc", 6);

	raytracer::initcomputeShader(width, height); // Initialize compute shader for lighting pass
	denoiser::initcomputeShader(width, height);

	initGlobalShaders();
	LightingHandler::setupShadowMapBuffer();
	// need to add debug buffers at some point
	//Framebuffer::setupNoiseMap();

	if (FEImGuiWindow::imGuiEnabled) {
		FEImGuiWindow::init();
		ImGuizmo::SetOrthographic(false);
		FEImGuiWindow::initImGui(windowHandler::window); // Initialize ImGUI
	}
	else
	{
		FEImGuiWindow::imGuiPanels[0] = false;
	}

	Skybox::init();

	WhiteCube = new CubeVisualizer;
	line = new Line3D(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
}

void RenderClass::initGlobalShaders() {
	// cube collider and billboard, oh yeah and framebuffer
	billBoardShader.LoadShader("Assets/Shaders/Db/BillBoard.vert", "Assets/Shaders/Db/BillBoard.frag");
	gPassShaderBillBoard.LoadShader("Assets/Shaders/gBuffer/geometryPassBillboard.vert", "Assets/Shaders/gBuffer/geometryPassBillboard.frag");
	boxShader.LoadShader("Assets/Shaders/Lighting/Default.vert", "Assets/Shaders/Db/OrangeHitbox.frag");
	SolidColour.LoadShader("Assets/Shaders/Lighting/Default.vert", "Assets/Shaders/Db/solidColour.frag");
	GBLpass.LoadShader("Assets/Shaders/Db/RenderQuad.vert", "Assets/Shaders/Db/RenderQuad.frag");


	LineShader.takePath = false;

	const char* lineVertexShaderSource =
		"#version 460 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"uniform mat4 lineMatrix;\n"
		"uniform vec3 aColour;\n"
		"uniform mat4 camMatrix;\n"
		"out vec3 colour;\n"
		"out vec3 crntPos;\n"
		"void main()\n"
		"{\n"
		"    crntPos = vec3(lineMatrix * vec4(aPos, 1.0f));\n"
		"    colour = aColour;\n"
		"    gl_Position = camMatrix * vec4(crntPos, 1.0f);\n"
		"}";

	const char* lineFragShaderSource =
		"#version 460 core\n"
		"out vec4 FragColor;\n"
		"in vec3 colour;\n"
		"void main()\n"
		"{\n"
		"    FragColor = vec4(colour, 1.0f);\n"
		"}";

	LineShader.LoadShader(
		lineVertexShaderSource,
		lineFragShaderSource
	);


	Framebuffer::frameBufferProgram.LoadShader("Assets/Shaders/PostProcess/framebuffer.vert", "Assets/Shaders/PostProcess/framebuffer.frag");
}

void RenderClass::ClearFramebuffers() {

	// Clear first framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::FBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // Clear with colour
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Clear second framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::FFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // Clear with colour
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Clear GBuffer
	glBindFramebuffer(GL_FRAMEBUFFER, GeometryPass::gBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear with colour
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, LightingHandler::shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

float Counter;
void RenderClass::Render(GLFWwindow* window, unsigned int width, unsigned int height) 
{
	// set clear colour
	glClearColor(RenderClass::gammaCorrect(skyRGBA.r), RenderClass::gammaCorrect(skyRGBA.g), RenderClass::gammaCorrect(skyRGBA.b), 1.0f);
	glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::FBO);
	glEnable(GL_DEPTH_TEST); // this line here caused me so much hell

	if (FEImGuiWindow::isWireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Enable wireframe mode
		glClearColor(pow(0.0f, Scene::maincamera.gamma), pow(0.0f, Scene::maincamera.gamma), pow(0.0f, Scene::maincamera.gamma), 1.0f);
	}

	if (!FEImGuiWindow::isWireframe && RenderClass::renderSkybox) // should add skybox.scene
		Skybox::draw();

	Scene::shadowmapDraw();
	Scene::draw();

	RenderHandler::render();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Restore normal rendering < wireframe
	if (DoDeferredLightingPass)
		DeferredLightingPass(); // Forward Lighting Pass
	if (DoComputeLightingPass)
	{
		if (raytracer::RTGlobalTransformFlag) raytracer::updateQuickModelData();
		raytracer::render(); // Run compute shader for lighting pass
		raytracer::RTGlobalTransformFlag = false;

	}

	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	// Framebuffer logic
	Framebuffer::FBODraw(FEImGuiWindow::imGuiPanels[0], window);

	if (FEImGuiWindow::imGuiEnabled) {
		Counter += TimeUtil::deltatime;
		if (Counter >= 1 / 10.0f) { if (glfwGetKey(windowHandler::window, GLFW_KEY_F1) == GLFW_PRESS) { FEImGuiWindow::imGuiPanels[0] = !FEImGuiWindow::imGuiPanels[0]; } Counter = 0; }
		if (FEImGuiWindow::imGuiPanels[0]) FEImGuiWindow::Update();
	}

	glfwSwapBuffers(window); // Swap BackBuffer with FrontBuffer (DoubleBuffering)
	glfwPollEvents(); // Tells open gl to proccess all events such as window resizing, inputs (KBM)
}

void RenderClass::DeferredLightingPass() {
	glDisable(GL_CULL_FACE);
	glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::FBO);
	GBLpass.Activate();
	// gPass textures bound to FB
	// send gPass textures to shader
	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, 0);

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

	GBLpass.setFloat3("fogColor", RenderClass::gammaCorrect3(fogRGBA));

	//mat4
	GBLpass.setMat4("cameraMatrix", Scene::maincamera.cameraMatrix);
	GBLpass.setMat4("projectionMatrix", Scene::maincamera.projection);

	glm::mat4 inverseProjection = glm::inverse(Scene::maincamera.projection);
	GBLpass.setMat4("inverseProjection", inverseProjection);

	glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(Scene::maincamera.cameraMatrix)));
	GBLpass.setMat3("normalMatrix", normalMatrix);

	GBLpass.setFloat3("orientation", Scene::maincamera.Orientation);
	GBLpass.setFloat3("cameraPos", Scene::maincamera.Position);
	GBLpass.setFloat3("cameraDirection", Scene::maincamera.Orientation);
	//std::cout << Camera::width << " " << Camera::height << std::endl;
	GBLpass.setFloat2("screenSize", glm::vec2(Scene::maincamera.width, Scene::maincamera.height));
	GBLpass.setFloat("time", glfwGetTime());
	//shader.
	lightingRenderQuad.draw();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void RenderClass::Cleanup() {
	billBoardShader.Delete();
	gPassShaderBillBoard.Delete();
	boxShader.Delete();
	GBLpass.Delete();
	SolidColour.Delete();
	billBoardShader.Delete();
	LineShader.Delete();
	Framebuffer::frameBufferProgram.Delete();

	WhiteCube->~CubeVisualizer();
	line->~Line3D();
}

float RenderClass::gammaCorrect(float input) {
	return pow(input, 1.0f / Scene::maincamera.gamma);
}

glm::vec3 RenderClass::gammaCorrect3(glm::vec3 input) {
	return pow(input, glm::vec3(1.0f / Scene::maincamera.gamma) );
}