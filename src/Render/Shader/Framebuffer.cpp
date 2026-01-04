#include "Render/Shader/Framebuffer.h"
#include <Render/passes/geometry/geometryPass.h>
#include <Render/passes/lighting/raytracer.h>
#include <Render/window/WindowHandler.h>
#include <utils/logConsole.h>
#include "Scene/scene.h"

int Framebuffer::tempWidth;
int Framebuffer::tempHeight;

unsigned int Framebuffer::ViewPortWidth = 800, Framebuffer::ViewPortHeight = 600;

Shader Framebuffer::frameBufferProgram;

unsigned int Framebuffer::FBO;
unsigned int Framebuffer::RBO;
unsigned int Framebuffer::texture;

unsigned int Framebuffer::FFBO;
unsigned int Framebuffer::FRBO;
unsigned int Framebuffer::Ftexture;

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

void Framebuffer::setupFBO(unsigned int width, unsigned int height) {
	// Initialize viewport rectangle object drawn to viewport with framebuffer texture attached
	rq.init();
	// GEN FBO
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	// GEN TEX and bind tex to fbo
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

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
	Framebuffer::ViewPortWidth = width;
	Framebuffer::ViewPortHeight = height;

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
	// update renderbuffer texture
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glBindTexture(GL_TEXTURE_2D, Ftexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	GeometryPass::updateGbufferResolution(width, height);

	raytracer::resizeTexture(width, height);
}

void Framebuffer::FBO2Draw() {
	glDisable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER, FFBO);
	glClear(GL_COLOR_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	rq.draw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Ftexture);
	rq.draw();

	glEnable(GL_DEPTH_TEST);
}

void ResizeLogic(GLFWwindow* window) {

		ScreenUtils::UpdateWindowResize(window);
		int newWidth, newHeight;
		glfwGetWindowSize(window, &newWidth, &newHeight);

	// we need a way to make isResizing == true when opengl window is resized
	if (ScreenUtils::isResizing == true) 
	{
		Framebuffer::updateFrameBufferResolution(newWidth, newHeight); // Update frame buffer resolution
		glViewport(0, 0, newWidth, newHeight);
		Scene::maincamera.SetViewportSize(newWidth, newHeight);
		ScreenUtils::isResizing = false;
	}
}

void Framebuffer::FBODraw(bool imGuiPanels, GLFWwindow* window) {

	if (!imGuiPanels) {
		ResizeLogic(window);
	}

	frameBufferProgram.Activate();

	frameBufferProgram.setMat4("view", Scene::maincamera.view);
	frameBufferProgram.setMat4("projection", Scene::maincamera.projection);
	frameBufferProgram.setMat4("cameraMatrix", Scene::maincamera.cameraMatrix);
	frameBufferProgram.setFloat("time", glfwGetTime());
	frameBufferProgram.setFloat("deltaTime", TimeUtil::deltatime);
	frameBufferProgram.setFloat("DepthDistance", RenderClass::DepthDistance);
	frameBufferProgram.setFloat("NearPlane", RenderClass::DepthPlane[0]);
	frameBufferProgram.setFloat("FarPlane", RenderClass::DepthPlane[1]);
	frameBufferProgram.setBool("doFog", RenderClass::doFog);
	frameBufferProgram.setFloat3("fogColor", RenderClass::gammaCorrect3(RenderClass::fogRGBA));

	// draw the framebuffer
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
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

	if (!imGuiPanels) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, ViewPortWidth, ViewPortHeight);

		glDisable(GL_DEPTH_TEST);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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
	glDeleteTextures(1, &texture);
	glDeleteFramebuffers(1, &FFBO);
	glDeleteRenderbuffers(1, &FRBO);
	glDeleteTextures(1, &Ftexture);
}