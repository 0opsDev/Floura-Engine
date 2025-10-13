#include "Render/Shader/Framebuffer.h"
#include <Render/passes/geometry/geometryPass.h>
#include <Render/passes/lighting/LightingPass.h>
#include <Render/window/WindowHandler.h>
#include <utils/logConsole.h>
#include <Scene/LightingHandler.h>

int Framebuffer::tempWidth;
int Framebuffer::tempHeight;

unsigned int Framebuffer::ViewPortWidth = 800, Framebuffer::ViewPortHeight = 600;

unsigned int Framebuffer::viewVAO, Framebuffer::viewVBO;
unsigned int Framebuffer::FBO2;

unsigned int Framebuffer::frameBufferTexture2, Framebuffer::frameBufferTexture;
unsigned int Framebuffer::RBO, Framebuffer::FBO;

GLuint Framebuffer::noiseMapTexture;
Shader Framebuffer::frameBufferProgram;

float s_ViewportVerticies[24] = {
	// Coords,   Texture cords
	 1.0f, -1.0f,  1.0f, 0.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	-1.0f,  1.0f,  0.0f, 1.0f,

	 1.0f,  1.0f,  1.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	-1.0f,  1.0f,  0.0f, 1.0f
};


void Framebuffer::setupNoiseMap() {
	// *TimeUtil::s_DeltaTime)
	NoiseH::generateNoise(noiseMapTexture, 256, 256, 0.05f, 42);
}

void Framebuffer::setupMainFBO(unsigned int width, unsigned int height) {
	// Initialize viewport rectangle object drawn to viewport with framebuffer texture attached
	glGenVertexArrays(1, &viewVAO);
	glGenBuffers(1, &viewVBO);
	glBindVertexArray(viewVAO);
	glBindBuffer(GL_ARRAY_BUFFER, viewVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(s_ViewportVerticies) * 6 * 4, s_ViewportVerticies, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	// FrameBuffer Object
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	// ColorBuffer
	glGenTextures(1, &frameBufferTexture);
	glBindTexture(GL_TEXTURE_2D, frameBufferTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameBufferTexture, 0);

	// Depth buffer
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);

	// Error checking
	auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
		if (init::LogALL || init::LogSystems) std::cout << "Framebuffer error: " << fboStatus << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::setupSecondFBO(unsigned int width, unsigned int height) {
	glGenFramebuffers(1, &FBO2);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO2);

	// Color buffer
	glGenTextures(1, &frameBufferTexture2);
	glBindTexture(GL_TEXTURE_2D, frameBufferTexture2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameBufferTexture2, 0);

	// Error checking
	auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
		if (init::LogALL || init::LogSystems) std::cout << "Framebuffer error: " << fboStatus << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::updateFrameBufferResolution(unsigned int width, unsigned int height) {
	Framebuffer::ViewPortWidth = width;
	Framebuffer::ViewPortHeight = height;

	// Update first frame buffer texture
	glBindTexture(GL_TEXTURE_2D, frameBufferTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Update second framebuffer texture
	glBindTexture(GL_TEXTURE_2D, frameBufferTexture2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
	GeometryPass::updateGbufferResolution(width, height);

	

	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	LightingPass::resizeTexture(width, height);
}

void Framebuffer::FBO2Draw() {
	// Apply post-processing and render to the second FBO
	glBindFramebuffer(GL_FRAMEBUFFER, FBO2);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	frameBufferProgram.Activate();
	glBindVertexArray(viewVAO);
	glDisable(GL_DEPTH_TEST);
	glBindTexture(GL_TEXTURE_2D, frameBufferTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// Copy the contents of the second FBO to the default FBO
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	frameBufferProgram.Activate();
	glBindVertexArray(viewVAO);
	glDisable(GL_DEPTH_TEST);
	glBindTexture(GL_TEXTURE_2D, frameBufferTexture2);
	glDrawArrays(GL_TRIANGLES, 0, 6);
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

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
	frameBufferProgram.Activate();
	frameBufferProgram.setMat4("view", Camera::view);
	frameBufferProgram.setMat4("projection", Camera::projection);
	frameBufferProgram.setMat4("cameraMatrix", Camera::cameraMatrix);
	frameBufferProgram.setFloat("time", glfwGetTime());
	frameBufferProgram.setFloat("deltaTime", TimeUtil::s_DeltaTime);
	frameBufferProgram.setBool("enableFB", FEImGuiWindow::enableFB);

	frameBufferProgram.setFloat("DepthDistance", RenderClass::DepthDistance);
	frameBufferProgram.setFloat("NearPlane", RenderClass::DepthPlane[0]);
	frameBufferProgram.setFloat("FarPlane", RenderClass::DepthPlane[1]);
	frameBufferProgram.setBool("doFog", RenderClass::doFog);
	frameBufferProgram.setFloat3("fogColor", RenderClass::fogRGBA.r, RenderClass::fogRGBA.g, RenderClass::fogRGBA.b);

	// draw the framebuffer
	glBindVertexArray(viewVAO);
	glDisable(GL_DEPTH_TEST); // stops culling on the rectangle the framebuffer is drawn on
	glBindTexture(GL_TEXTURE_2D, frameBufferTexture);

	frameBufferProgram.Activate();

	//noiseMapTexture

	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, noiseMapTexture);
	frameBufferProgram.setInt("noiseMapTexture", 7);

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

	if (!imGuiPanels) {


		frameBufferProgram.Activate();
		frameBufferProgram.setInt("depthMap", 5);

		ResizeLogic(imGuiPanels, window, windowHandler::window_width, windowHandler::window_height);

		glDrawArrays(GL_TRIANGLES, 0, 6);
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