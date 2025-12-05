#include "Render/Shader/Framebuffer.h"
#include <Render/passes/geometry/geometryPass.h>
#include <Render/passes/lighting/LightingPass.h>
#include <Render/window/WindowHandler.h>
#include <utils/logConsole.h>

int Framebuffer::tempWidth;
int Framebuffer::tempHeight;

unsigned int Framebuffer::ViewPortWidth = 800, Framebuffer::ViewPortHeight = 600;

//unsigned int Framebuffer::viewVAO, Framebuffer::viewVBO;
//unsigned int Framebuffer::FBO2;

//unsigned int Framebuffer::frameBufferTexture2; //Framebuffer::frameBufferTexture;
//unsigned int Framebuffer::RBO, Framebuffer::FBO;

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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

		glGenRenderbuffers(1, &RBO);
		glBindRenderbuffer(GL_RENDERBUFFER, RBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);

	// Error checking
	auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer error: " << fboStatus << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// GEN FBO
	glGenFramebuffers(1, &FFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FFBO);
	// GEN TEX and bind tex to fbo
	glGenTextures(1, &Ftexture);
	glBindTexture(GL_TEXTURE_2D, Ftexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
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

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Framebuffer::updateFrameBufferResolution(unsigned int width, unsigned int height) {
	Framebuffer::ViewPortWidth = width;
	Framebuffer::ViewPortHeight = height;

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
	// update renderbuffer texture
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glBindTexture(GL_TEXTURE_2D, Ftexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	GeometryPass::updateGbufferResolution(width, height);

	LightingPass::resizeTexture(width, height);
}

void Framebuffer::FBO2Draw() {
	// Apply post-processing and render to the second FBO
	glBindFramebuffer(GL_FRAMEBUFFER, FFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	frameBufferProgram.Activate();
	glDisable(GL_DEPTH_TEST);
	glBindTexture(GL_TEXTURE_2D, texture);
	// here
	glDrawArrays(GL_TRIANGLES, 0, 6);
	rq.draw(frameBufferProgram);
	// Copy the contents of the second FBO to the default FBO
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	frameBufferProgram.Activate();
	glDisable(GL_DEPTH_TEST);
	glBindTexture(GL_TEXTURE_2D, Ftexture);
	rq.draw(frameBufferProgram);
};

float current_width = 0;
float current_height = 0;

void ResizeLogic(bool imGuiPanels, GLFWwindow* window, unsigned int Vwidth,
	unsigned int Vheight) {
	if (!imGuiPanels) {
		ScreenUtils::UpdateWindowResize(window);
		int newWidth, newHeight;
		glfwGetFramebufferSize(window, &newWidth, &newHeight);

		current_width = newWidth;
		current_height = newHeight;
		//camera.UpdateRes(newWidth, newHeight);
		

	}
	// we need a way to make isResizing == true when opengl window is resized
	if (ScreenUtils::isResizing == true) {
		//std::cout << "Resolution scale changed!" << std::endl;
		Framebuffer::updateFrameBufferResolution(current_width, current_height); // Update frame buffer resolution
		glViewport(0, 0, (current_width), (current_height ));
		Camera::SetViewportSize(current_width, current_height);
		//std::cout << "External camera instance address: " << &camera << std::endl;
		//std::cout << current_width << " " << camera.width << std::endl;
		//std::cout << current_height << " " << camera.height << std::endl;
	}
}

void Framebuffer::FBODraw(bool imGuiPanels, GLFWwindow* window) {

	// Switch back to the normal depth function
	glDepthFunc(GL_LESS);

	// draw main fbo, then we should just draw fbo2

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
	frameBufferProgram.Activate();
	frameBufferProgram.setMat4("view", Camera::view);
	frameBufferProgram.setMat4("projection", Camera::projection);
	frameBufferProgram.setMat4("cameraMatrix", Camera::cameraMatrix);
	frameBufferProgram.setFloat("time", glfwGetTime());
	frameBufferProgram.setFloat("deltaTime", TimeUtil::s_DeltaTime);

	frameBufferProgram.setFloat("DepthDistance", RenderClass::DepthDistance);
	frameBufferProgram.setFloat("NearPlane", RenderClass::DepthPlane[0]);
	frameBufferProgram.setFloat("FarPlane", RenderClass::DepthPlane[1]);
	frameBufferProgram.setBool("doFog", RenderClass::doFog);
	frameBufferProgram.setFloat3("fogColor", RenderClass::gammaCorrect3(RenderClass::fogRGBA));

	// draw the framebuffer
	glDisable(GL_DEPTH_TEST); // stops culling on the rectangle the framebuffer is drawn on
	glBindTexture(GL_TEXTURE_2D, texture);

	frameBufferProgram.Activate();

	// gPass textures bound to FB
	// send gPass textures to shader
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

	frameBufferProgram.setFloat("gamma", Camera::gamma);

	if (!imGuiPanels) {


		frameBufferProgram.Activate();
		frameBufferProgram.setInt("depthMap", 5);

		ResizeLogic(imGuiPanels, window, windowHandler::window_width, windowHandler::window_height);

		rq.draw(frameBufferProgram);
	}
	else{
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, GeometryPass::depthTexture);
		frameBufferProgram.Activate();
		frameBufferProgram.setInt("depthMap", 5);
		// copy contents of FB to FB2 and Display FB2
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