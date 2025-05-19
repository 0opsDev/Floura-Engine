#include "Render/Shader/Framebuffer.h"
#include <utils/SettingsUtil.h>

unsigned int Framebuffer::ViewPortWidth = 800;
unsigned int Framebuffer::ViewPortHeight = 600;
unsigned int Framebuffer::viewVAO;
unsigned int Framebuffer::viewVBO;
unsigned int Framebuffer::FBO2;
unsigned int Framebuffer::frameBufferTexture2;
unsigned int Framebuffer::RBO2;
unsigned int Framebuffer::frameBufferTexture;
unsigned int Framebuffer::RBO;
unsigned int Framebuffer::FBO;

void Framebuffer::setupMainFBO(unsigned int width, unsigned int height) {
	// Initialize viewport rectangle object drawn to viewport with framebuffer texture attached
	glGenVertexArrays(1, &viewVAO);
	glGenBuffers(1, &viewVBO);
	glBindVertexArray(viewVAO);
	glBindBuffer(GL_ARRAY_BUFFER, viewVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(SettingsUtils::s_ViewportVerticies) * 6 * 4, SettingsUtils::s_ViewportVerticies, GL_STATIC_DRAW);
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

	// DepthBuffer + StencilBuffer
	// RenderBuffer Object
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

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

	// Depth and stencil buffer
	glGenRenderbuffers(1, &RBO2);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO2);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO2);

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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (width * ImGuiCamera::resolutionScale), (height * ImGuiCamera::resolutionScale), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	if (ImGuiCamera::enableLinearScaling) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	// Update first render buffer storage
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, (width * ImGuiCamera::resolutionScale), (height * ImGuiCamera::resolutionScale));
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// Update second frame buffer texture
	glBindTexture(GL_TEXTURE_2D, frameBufferTexture2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (width * ImGuiCamera::resolutionScale), (height * ImGuiCamera::resolutionScale), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	if (ImGuiCamera::enableLinearScaling) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	// Update second render buffer storage
	glBindRenderbuffer(GL_RENDERBUFFER, RBO2);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, (width * ImGuiCamera::resolutionScale), (height * ImGuiCamera::resolutionScale));
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void Framebuffer::FBO2Draw(Shader frameBufferProgram) {
	// Apply post-processing and render to the second FBO
	glBindFramebuffer(GL_FRAMEBUFFER, FBO2);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	frameBufferProgram.Activate();
	glBindVertexArray(viewVAO);
	glDisable(GL_DEPTH_TEST);
	glBindTexture(GL_TEXTURE_2D, frameBufferTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// Copy the contents of the second FBO to the default FBO
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
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
	if (ScreenUtils::isResizing == true || ImGuiCamera::resolutionScale != ImGuiCamera::prevResolutionScale || ImGuiCamera::enableLinearScaling != ImGuiCamera::prevEnableLinearScaling) {
		std::cout << "Resolution scale changed!" << std::endl;
		Framebuffer::updateFrameBufferResolution(current_width, current_height); // Update frame buffer resolution
		glViewport(0, 0, (current_width * ImGuiCamera::resolutionScale), (current_height * ImGuiCamera::resolutionScale));
		Camera::SetViewportSize(current_width, current_height);
		//std::cout << "External camera instance address: " << &camera << std::endl;
		//std::cout << current_width << " " << camera.width << std::endl;
		//std::cout << current_height << " " << camera.height << std::endl;
	}
	ImGuiCamera::prevResolutionScale = ImGuiCamera::resolutionScale; // Update the previous scale
	ImGuiCamera::prevEnableLinearScaling = ImGuiCamera::ImGuiCamera::enableLinearScaling;
}

void Framebuffer::FBODraw(
	Shader frameBufferProgram,
	bool imGuiPanels, unsigned int Vwidth, unsigned int Vheight, GLFWwindow* window) {
	// Switch back to the normal depth function
	glDepthFunc(GL_LESS);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	frameBufferProgram.Activate();
	UF::Float(frameBufferProgram.ID, "time", glfwGetTime());
	UF::Float(frameBufferProgram.ID, "deltaTime", TimeUtil::s_DeltaTime);

	UF::Float(frameBufferProgram.ID, ImGuiCamera::UniformInput, ImGuiCamera::UniformFloat[0]);
	UF::Bool(frameBufferProgram.ID, "enableFB", ImGuiCamera::enableFB);

	// draw the framebuffer
	glBindVertexArray(viewVAO);
	glDisable(GL_DEPTH_TEST); // stops culling on the rectangle the framebuffer is drawn on
	glBindTexture(GL_TEXTURE_2D, frameBufferTexture);

	if (!imGuiPanels) {
		ResizeLogic(imGuiPanels, window, Vwidth, Vheight);

		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	else{
		// copy contents of FB to FB2 and Display FB2
		Framebuffer::FBO2Draw(frameBufferProgram);
		//(Shader frameBufferProgram, unsigned int& frameBufferTexture, unsigned int& frameBufferTexture2, unsigned int& FBO2)
	}

}