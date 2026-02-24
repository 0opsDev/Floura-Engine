#include "Render/Shader/Framebuffer.h"
#include <Render/passes/geometry/geometryPass.h>
#include <Render/passes/lighting/raytracer.h>
#include <Render/window/WindowHandler.h>
#include <utils/logConsole.h>
#include "Scene/scene.h"
#include <glm/gtx/compatibility.hpp>
#include <Render/passes/post/historyPass.h>
#include "Render/passes/dbg/dbgPass.h"

int Framebuffer::tempWidth;
int Framebuffer::tempHeight;

unsigned int Framebuffer::ViewPortWidth = 800, Framebuffer::ViewPortHeight = 600;

Shader Framebuffer::frameBufferProgram;

unsigned int Framebuffer::FBO;
unsigned int Framebuffer::RBO;
unsigned int Framebuffer::screentexture;

unsigned int Framebuffer::FFBO;
unsigned int Framebuffer::FRBO;
unsigned int Framebuffer::Ftexture;

unsigned int Framebuffer::cmFBO;
unsigned int Framebuffer::cmRBO;
unsigned int Framebuffer::cmtexture;


RenderQuad Framebuffer::rq;

float s_ViewportVerticies[24] = {
	// Coords,   Texture cords
	 1.0f, -1.0f,  1.0f, 0.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	-1.0f,  1.0f,  0.0f, 1.0f,

	 1.0f,  1.0f,  1.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	-1.0f,  1.0f,  0.0f, 1.0f
};


void Framebuffer::smInit(glm::vec2 res)
{
	// GEN FBO
	glGenFramebuffers(1, &Framebuffer::cmFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::cmFBO);
	// GEN TEX and bind tex to fbo
	glGenTextures(1, &Framebuffer::cmtexture);
	glBindTexture(GL_TEXTURE_2D, Framebuffer::cmtexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, res.x, res.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Framebuffer::cmtexture, 0);

	glGenRenderbuffers(1, &Framebuffer::cmRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, Framebuffer::cmRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, res.x, res.y);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, Framebuffer::cmRBO);


	// Error checking
	auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer error: " << fboStatus << std::endl;
	}
}

void Framebuffer::smUpdateResolution(glm::vec2 res)
{
	glBindTexture(GL_TEXTURE_2D, Framebuffer::cmtexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, res.x, res.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
	// update renderbuffer texture
	glBindRenderbuffer(GL_RENDERBUFFER, Framebuffer::cmRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, res.x, res.y);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}


void Framebuffer::clearsmbuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::cmFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void Framebuffer::setupFBO(unsigned int width, unsigned int height) {
	// Initialize viewport rectangle object drawn to viewport with framebuffer texture attached
	rq.init();
	// GEN FBO
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	// GEN TEX and bind tex to fbo
	glGenTextures(1, &screentexture); // GL_LINEAR
	glBindTexture(GL_TEXTURE_2D, screentexture); // GL_UNSIGNED_BYTE
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // GL_LINEAR_MIPMAP_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screentexture, 0);

	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);


	// Error checking
	auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer error: " << fboStatus << std::endl;
	}


	// GEN FBO
	glGenFramebuffers(1, &FFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FFBO);
	// GEN TEX and bind tex to fbo
	glGenTextures(1, &Ftexture);
	glBindTexture(GL_TEXTURE_2D, Ftexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Ftexture, 0);

	// Error checking
	auto fboStatus2 = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus2 != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer error: " << fboStatus2 << std::endl;
	}

}
void Framebuffer::updateFrameBufferResolution(unsigned int width, unsigned int height) {
	
	//	static unsigned int ViewPortWidth, ViewPortHeight;
	if (width == ViewPortWidth && height == ViewPortHeight ) return;
	
	//std::cout << "e" <<std::endl;
	
	Framebuffer::ViewPortWidth = width;
	Framebuffer::ViewPortHeight = height;
	
	glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));

	Scene::maincamera.SetViewportSize(static_cast<int>(width), static_cast<int>(height));


	glBindTexture(GL_TEXTURE_2D, screentexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
	// update renderbuffer texture
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glBindTexture(GL_TEXTURE_2D, Ftexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	dbgPass::updateDBGResolution(width, height);
	GeometryPass::updateGbufferResolution(width, height);
	HistoryPass::updateHbufferResolution(width, height);

	raytracer::resizeTexture(width, height);
}

void Framebuffer::FBO2Draw() {
	glDisable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER, FFBO);
	glClear(GL_COLOR_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, screentexture);
	rq.draw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Ftexture);
	rq.draw();

	glEnable(GL_DEPTH_TEST);
}

void ResizeLogic(GLFWwindow* window) {
		int newWidth, newHeight;
		glfwGetWindowSize(window, &newWidth, &newHeight);

	// we need a way to make isResizing == true when opengl window is resized
		Framebuffer::updateFrameBufferResolution(newWidth, newHeight); // Update frame buffer resolution
		//glViewport(0, 0, newWidth, newHeight);
		//Scene::maincamera.SetViewportSize(newWidth, newHeight);
}

void Framebuffer::FBODraw(bool imGuiPanels, GLFWwindow* window) {

	glBindTexture(GL_TEXTURE0, 0);

	if (!imGuiPanels) {
		ResizeLogic(window);
	}

	frameBufferProgram.Activate();

	frameBufferProgram.setMat4("view", Scene::maincamera.view);
	frameBufferProgram.setMat4("projection", Scene::maincamera.projection);
	frameBufferProgram.setMat4("cameraMatrix", Scene::maincamera.cameraMatrix);
	frameBufferProgram.setFloat("time", glfwGetTime());
	frameBufferProgram.setFloat("deltaTime", TimeUtil::deltatime);
	frameBufferProgram.setBool("overlayDebug", dbgPass::overlayDebug);

	// draw the framebuffer
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, screentexture);
	glGenerateMipmap(GL_TEXTURE_2D);
	frameBufferProgram.setInt("screenTexture", 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, GeometryPass::gPosition);
	frameBufferProgram.setInt("gPosition", 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, GeometryPass::gNormal);
	frameBufferProgram.setInt("gNormal", 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, GeometryPass::gAlbedoSpec);
	frameBufferProgram.setInt("gAlbedoSpec", 3);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, GeometryPass::depthTexture);

	frameBufferProgram.setFloat("gamma", Scene::maincamera.gamma);

	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, GeometryPass::depthTexture);
	frameBufferProgram.setInt("depthMap", 6);

	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, GeometryPass::gSpecular);
	frameBufferProgram.setInt("gSpecular", 7);
	
	glActiveTexture(GL_TEXTURE9);
	glBindTexture(GL_TEXTURE_2D, GeometryPass::gVelocity);
	frameBufferProgram.setInt("gVelocity", 9);
	
	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D, dbgPass::dbgColour);
	frameBufferProgram.setInt("dbgColour", 10);
	//dbgPass

	if (!imGuiPanels) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, ViewPortWidth, ViewPortHeight);

		glDisable(GL_DEPTH_TEST);
		
		glClearColor(RenderClass::gammaCorrect(RenderClass::skyRGBA.r), RenderClass::gammaCorrect(RenderClass::skyRGBA.g), RenderClass::gammaCorrect(RenderClass::skyRGBA.b), 1.0f);
		if (FEImGuiWindow::isWireframe)
			glClearColor(pow(0.0f, Scene::maincamera.gamma), pow(0.0f, Scene::maincamera.gamma), pow(0.0f, Scene::maincamera.gamma), 1.0f);
		
		glClear(GL_COLOR_BUFFER_BIT);

		rq.draw();
		glEnable(GL_DEPTH_TEST);
	}
	else
	{
		frameBufferProgram.Activate();
		Framebuffer::FBO2Draw();
	}
}

void Framebuffer::Delete()
{
	glDeleteFramebuffers(1, &FBO);
	glDeleteRenderbuffers(1, &RBO);
	glDeleteTextures(1, &screentexture);
	glDeleteFramebuffers(1, &FFBO);
	glDeleteRenderbuffers(1, &FRBO);
	glDeleteTextures(1, &Ftexture);
}