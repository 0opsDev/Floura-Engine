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
#include  "Render/passes/post/historyPass.h"
#include <Render/Handler/RenderHandler.h>
#include  "Render/Handler/sceneDescription.h"
#include "Render/passes/dbg/dbgPass.h"
#include "Scene/FE_LAYER.h"

Shader RenderClass::GBLpass;
Shader RenderClass::ssPass;
Shader RenderClass::taaShader;
Shader RenderClass::billBoardShader;
Shader RenderClass::gPassShaderBillBoard;
Shader RenderClass::boxShader;
Shader RenderClass::LineShader;

bool RenderClass::renderSkybox = true;
bool RenderClass::doReflections = true;
bool RenderClass::doSSR = true;
bool RenderClass::doContactShadows = true;
bool RenderClass::doFog = true;
GLfloat RenderClass::DepthDistance = 100.0f;
GLfloat RenderClass::DepthPlane[] = { 0.1f, 100.0f };
glm::vec3 RenderClass::skyRGBA = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 RenderClass::fogRGBA = glm::vec3( 1.0f, 1.0f, 1.0f);

CubeVisualizer* RenderClass::WhiteCube;
Line3D* RenderClass::line;
Texture* RenderClass::bluenoise;
Texture* RenderClass::bayermatrix;
bool RenderClass::doTAA = true;
bool RenderClass::doBinaryAlpha = true;
bool RenderClass::animateBinaryAlpha = true;

Shader SolidColour;
RenderQuad lightingRenderQuad;

bool RenderClass::DoDeferredLightingPass = true; // Toggle for lighting pass
bool RenderClass::DoForwardLightingPass = false; // Toggle for regular pass
bool RenderClass::DoComputeLightingPass = false;

void initGLenable() {
	// glenables
	// depth pass. render things in correct order. eg sky behind wall, dirt under water, not random order
	glEnable(GL_DEPTH_TEST); // Depth buffer
	glDepthFunc(GL_LESS);
	//glEnable(GL_STENCIL_TEST); //stencil buffer
	//glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glEnable(GL_CULL_FACE); // Culling
	glCullFace(GL_BACK);

	//switch (frontFaceSide) { //currently set to false
	//case true: { glFrontFace(GL_CW); break; } // inside facing
	//case false: { glFrontFace(GL_CCW); break; } // outside facing
	//}

	glFrontFace(GL_CCW);
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

	windowHandler::InitMainWindow();
	if (!gladLoadGL(glfwGetProcAddress)) {
		// Log that GLAD failed to initialize
		std::cerr << "Failed to initialize GLAD" << std::endl;
		return;
	}



	windowHandler::setVSync(windowHandler::doVsync); // Set Vsync to value of doVsync (bool)

	// glenables
	// depth pass. render things in correct order. eg sky behind wall, dirt under water, not random order
	initGLenable(); //bool for direction of polys
	HistoryPass::init(); // init the RQ of the HP
	lightingRenderQuad.init();
	// put in one function
	Framebuffer::setupFBO(width, height);
	dbgPass::setupDBGbuffers(width, height);
	GeometryPass::setupGbuffers(width, height); // here
	HistoryPass::setupHbuffers(width, height);
	SceneDescription::generateSceneBuffers();

	// load bluenoise texture
	bluenoise = new Texture(); bluenoise->createTexture("Assets/Dependants/LDR_LLL1_0.png", "misc", 6);
	bayermatrix = new Texture(); bayermatrix->createTexture("Assets/Dependants/bayer_matrix.png", "misc", 7);

	raytracer::initcomputeShader(width, height); // Initialize compute shader for lighting pass
	denoiser::initcomputeShader(width, height);

	initGlobalShaders();
	LightingHandler::setupShadowMapBuffer();
	// need to add debug buffers at some point
	//Framebuffer::setupNoiseMap();


	Framebuffer::smInit(glm::vec2(128));
	RenderHandler::init();

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
	
	// Assets/Shaders/gBuffer/historybuffer
	// history pass
	HistoryPass::hPassShader.LoadShader("Assets/Shaders/gBuffer/historybuffer.vert", "Assets/Shaders/gBuffer/historybuffer.frag");
	
	// keep an eye on these in a moment or later
	
	//GBLpass.LoadShader("Assets/Shaders/Db/RenderQuad.vert", "Assets/Shaders/Db/RenderQuad.frag");
	GBLpass.LoadShader("Assets/Shaders/Deferred/DFR_Phon.vert", "Assets/Shaders/Deferred/DFR_Phon.frag");
	ssPass.LoadShader("Assets/Shaders/Deferred/screenspaceReflections.vert", "Assets/Shaders/Deferred/screenspaceReflections.frag");
	taaShader.LoadShader("Assets/Shaders/PostProcess/TAA.vert", "Assets/Shaders/PostProcess/TAA.frag");

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
	
	glBindFramebuffer(GL_FRAMEBUFFER,  dbgPass::dbgBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear with colour
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	LightingHandler::clearSMFBO();
}

float Counter;
void RenderClass::Render(GLFWwindow* window, unsigned int width, unsigned int height) 
{
	LightingHandler::update();
	RenderClass::ClearFramebuffers(); // Clear Framebuffers
	
	// set clear colour
	glClearColor(RenderClass::gammaCorrect(skyRGBA.r), RenderClass::gammaCorrect(skyRGBA.g), RenderClass::gammaCorrect(skyRGBA.b), 1.0f);
	glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::FBO);
	glEnable(GL_DEPTH_TEST); // this line here caused me so much hell

	if (FEImGuiWindow::isWireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Enable wireframe mode
		glClearColor(pow(0.0f, Scene::maincamera.gamma), pow(0.0f, Scene::maincamera.gamma), pow(0.0f, Scene::maincamera.gamma), 1.0f);
	}

	if (!FEImGuiWindow::isWireframe && RenderClass::renderSkybox) // should add skybox.scene
	{
		Skybox::draw(Scene::maincamera, Framebuffer::FBO, true);
		Skybox::setPreviousMats(Scene::maincamera);
		glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::FBO);
	}

	Scene::shadowmapDraw();
	
	Scene::draw();
	
	RenderHandler::render();
	
	// Framebuffer logic
	Framebuffer::FBODraw(FEImGuiWindow::imGuiPanels[0], window);

	if (FEImGuiWindow::imGuiEnabled) {
		Counter += TimeUtil::deltatime;
		if (Counter >= 1 / 10.0f) { if (glfwGetKey(windowHandler::window, GLFW_KEY_F1) == GLFW_PRESS) { FEImGuiWindow::imGuiPanels[0] = !FEImGuiWindow::imGuiPanels[0]; } Counter = 0; }
		if (FEImGuiWindow::imGuiPanels[0]) FEImGuiWindow::Update();
	}

	glfwSwapBuffers(window); // Swap BackBuffer with FrontBuffer (DoubleBuffering)
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
	
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, GeometryPass::gSpecular);
	GBLpass.setInt("gSpecular", 6);
	
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, GeometryPass::gVelocity);
	GBLpass.setInt("gVelocity", 7);
	// skip 8 because of shadow map (i really need to use bindless on these)
	
	
	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D, GeometryPass::gEmission);
	glGenerateMipmap(GL_TEXTURE_2D);
	GBLpass.setInt("gEmission", 10);
	

	glActiveTexture(GL_TEXTURE11);
	glBindTexture(GL_TEXTURE_2D, HistoryPass::hColour);
	glGenerateMipmap(GL_TEXTURE_2D); // remove later
	GBLpass.setInt("hColour", 11);
	
	// reserve 10 for depth
	glActiveTexture(GL_TEXTURE12);
	glBindTexture(GL_TEXTURE_2D, HistoryPass::hDepthTexture);
	GBLpass.setInt("hDepthTexture", 12);
	
	// prior normals
	glActiveTexture(GL_TEXTURE13);
	glBindTexture(GL_TEXTURE_2D, HistoryPass::hNormal);
	GBLpass.setInt("hNormal", 13);
	
	
	GBLpass.setFloat("NearPlane", Scene::maincamera.nearFar.x);
	GBLpass.setFloat("FarPlane", Scene::maincamera.nearFar.y);

	GBLpass.setFloat("fogDepthDistance", DepthDistance);
	GBLpass.setFloat("fogNearPlane", DepthPlane[0]);
	GBLpass.setFloat("fogFarPlane", DepthPlane[1]);
	GBLpass.setBool("doFog", doFog);

	GBLpass.setFloat3("fogColor", RenderClass::gammaCorrect3(fogRGBA));
	GBLpass.Activate();
	//mat4
	GBLpass.setMat4("cameraMatrix", Scene::maincamera.cameraMatrix);
	GBLpass.setMat4("projectionMatrix", Scene::maincamera.projection);
	GBLpass.setMat4("viewMatrix", Scene::maincamera.view);
	glm::mat4 inverseView = glm::inverse(Scene::maincamera.view);
	GBLpass.setMat4("inverseViewMatrix",inverseView);

	glm::mat4 inverseProjection = glm::inverse(Scene::maincamera.projection);
	GBLpass.setMat4("inverseProjection", inverseProjection);

	glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(Scene::maincamera.cameraMatrix)));
	GBLpass.setMat3("normalMatrix", normalMatrix);

	GBLpass.setFloat3("orientation", Scene::maincamera.Orientation);
	GBLpass.setFloat3("cameraPos", Scene::maincamera.Position);
	GBLpass.setFloat3("cameraDirection", Scene::maincamera.Orientation);
	//std::cout << Camera::width << " " << Camera::height << std::endl;
	GBLpass.setFloat2("screenSize", glm::vec2(Scene::maincamera.width, Scene::maincamera.height));
	
	GBLpass.setFloat3("camPos", Scene::maincamera.Position);
	GBLpass.setInt("indirectSamples", ProbeHandler::indirectSamples);

	
	GBLpass.setHandleui64ARB("BlueNoiseHandle", RenderClass::bluenoise->handle);
	GBLpass.setHandleui64ARB("bayerMatrixHandle", RenderClass::bayermatrix->handle);

	GBLpass.setBool("doSSR", RenderClass::doSSR);
	GBLpass.setBool("doContactShadows", RenderClass::doContactShadows);

	GBLpass.setTimeVariables();
	
	LightingHandler::sendToShader(GBLpass);
	
	//shader.
	lightingRenderQuad.draw();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void RenderClass::ScreenSpaceLightingPass()
{
		glDisable(GL_CULL_FACE);
	glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::FBO);
	ssPass.Activate();
	// gPass textures bound to FB
	// send gPass textures to shader
	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Framebuffer::screentexture);
	taaShader.setInt("screentexture", 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, GeometryPass::gPosition);
	ssPass.setInt("gPosition", 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, GeometryPass::gNormal);
	ssPass.setInt("gNormal", 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, GeometryPass::gAlbedoSpec);
	ssPass.setInt("gAlbedoSpec", 3);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, GeometryPass::depthTexture);
	ssPass.setInt("depthMap", 5);
	
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, GeometryPass::gSpecular);
	ssPass.setInt("gSpecular", 6);
	
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, GeometryPass::gVelocity);
	ssPass.setInt("gVelocity", 7);
	// skip 8 because of shadow map (i really need to use bindless on these)
	
	
	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D, GeometryPass::gEmission);
	glGenerateMipmap(GL_TEXTURE_2D);
	ssPass.setInt("gEmission", 10);
	

	glActiveTexture(GL_TEXTURE11);
	glBindTexture(GL_TEXTURE_2D, HistoryPass::hColour);
	glGenerateMipmap(GL_TEXTURE_2D); // remove later
	ssPass.setInt("hColour", 11);
	
	// reserve 10 for depth
	glActiveTexture(GL_TEXTURE12);
	glBindTexture(GL_TEXTURE_2D, HistoryPass::hDepthTexture);
	ssPass.setInt("hDepthTexture", 12);
	
	// prior normals
	glActiveTexture(GL_TEXTURE13);
	glBindTexture(GL_TEXTURE_2D, HistoryPass::hNormal);
	ssPass.setInt("hNormal", 13);
	
	
	ssPass.setFloat("NearPlane", Scene::maincamera.nearFar.x);
	ssPass.setFloat("FarPlane", Scene::maincamera.nearFar.y);

	ssPass.setFloat("fogDepthDistance", DepthDistance);
	ssPass.setFloat("fogNearPlane", DepthPlane[0]);
	ssPass.setFloat("fogFarPlane", DepthPlane[1]);
	ssPass.setBool("doFog", doFog);

	ssPass.setFloat3("fogColor", RenderClass::gammaCorrect3(fogRGBA));
	ssPass.Activate();
	//mat4
	ssPass.setMat4("cameraMatrix", Scene::maincamera.cameraMatrix);
	ssPass.setMat4("projectionMatrix", Scene::maincamera.projection);
	ssPass.setMat4("viewMatrix", Scene::maincamera.view);
	glm::mat4 inverseView = glm::inverse(Scene::maincamera.view);
	ssPass.setMat4("inverseViewMatrix",inverseView);

	glm::mat4 inverseProjection = glm::inverse(Scene::maincamera.projection);
	ssPass.setMat4("inverseProjection", inverseProjection);

	glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(Scene::maincamera.cameraMatrix)));
	ssPass.setMat3("normalMatrix", normalMatrix);

	ssPass.setFloat3("orientation", Scene::maincamera.Orientation);
	ssPass.setFloat3("cameraPos", Scene::maincamera.Position);
	ssPass.setFloat3("cameraDirection", Scene::maincamera.Orientation);
	//std::cout << Camera::width << " " << Camera::height << std::endl;
	ssPass.setFloat2("screenSize", glm::vec2(Scene::maincamera.width, Scene::maincamera.height));
	
	ssPass.setFloat3("camPos", Scene::maincamera.Position);
	ssPass.setInt("indirectSamples", ProbeHandler::indirectSamples);

	
	ssPass.setHandleui64ARB("BlueNoiseHandle", RenderClass::bluenoise->handle);
	ssPass.setHandleui64ARB("bayerMatrixHandle", RenderClass::bayermatrix->handle);

	ssPass.setBool("doSSR", RenderClass::doSSR);
	ssPass.setBool("doContactShadows", RenderClass::doContactShadows);

	ssPass.setTimeVariables();
	
	LightingHandler::sendToShader(ssPass);
	
	//shader.
	lightingRenderQuad.draw();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void RenderClass::taaPass()
{
		glDisable(GL_CULL_FACE);
	glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::FBO);
	taaShader.Activate();
	// gPass textures bound to FB
	// send gPass textures to shader
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Framebuffer::screentexture);
	taaShader.setInt("screentexture", 0);
	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, GeometryPass::gNormal);
	taaShader.setInt("gNormal", 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, GeometryPass::depthTexture);
	taaShader.setInt("depthMap", 2);
	
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, GeometryPass::gVelocity);
	taaShader.setInt("gVelocity", 3);

	// skip 8 because of shadow map (i really need to use bindless on these)
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, HistoryPass::hColour);
	taaShader.setInt("hColour", 4);
	
	// reserve 10 for depth
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, HistoryPass::hDepthTexture);
	taaShader.setInt("hDepthTexture", 5);
	
	// prior normals
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, HistoryPass::hNormal);
	taaShader.setInt("hNormal", 6);
	
	taaShader.setFloat("NearPlane", Scene::maincamera.nearFar.x);
	taaShader.setFloat("FarPlane", Scene::maincamera.nearFar.y);
	
	taaShader.setFloat2("screenSize", glm::vec2(Scene::maincamera.width, Scene::maincamera.height));
	taaShader.setTimeVariables();
	
	
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
	taaShader.Delete();
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

void RenderClass::compileShaders()
{
	for (int i = 0; i < Scene::entityObjects.size(); ++i) Scene::entityObjects[i]->component.systems.material.Material.compileUniforms();
}
