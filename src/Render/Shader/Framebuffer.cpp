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
unsigned int Framebuffer::depthTexture;
unsigned int Framebuffer::depthTexture2;
unsigned int Framebuffer::gBuffer;
unsigned int Framebuffer::gAlbedoSpec;
unsigned int Framebuffer::gNormal;
unsigned int Framebuffer::gPosition;
unsigned int Framebuffer::DBO;
Shader gPassShader("skip", "");

void Framebuffer::setupGbuffers(unsigned int width, unsigned int height) {
	gPassShader = Shader("Shaders/gBuffer/geometryPass.vert", "Shaders/gBuffer/geometryPass.frag");
	//generate buffer in memory and bind
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	// - position color buffer
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gPosition, 0);


	// - normal color buffer
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gNormal, 0);

	glGenTextures(1, &gAlbedoSpec);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gAlbedoSpec, 0);

	glGenRenderbuffers(1, &DBO);
	glBindRenderbuffer(GL_RENDERBUFFER, DBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, DBO);

	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(3, attachments);

	    // finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

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
	//glGenRenderbuffers(1, &RBO);
//	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

	// Create Depth Texture **(NEW)**
	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

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

	glGenTextures(1, &depthTexture2);
	glBindTexture(GL_TEXTURE_2D, depthTexture2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture2, 0);

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

	// Apply texture filtering settings
	if (ImGuiCamera::enableLinearScaling) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	// Update second framebuffer texture
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

	// Update depth textures
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width * ImGuiCamera::resolutionScale, height * ImGuiCamera::resolutionScale, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
	glBindTexture(GL_TEXTURE_2D, depthTexture2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width * ImGuiCamera::resolutionScale, height * ImGuiCamera::resolutionScale, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);

	if (ImGuiCamera::enableLinearScaling) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width * ImGuiCamera::resolutionScale, height * ImGuiCamera::resolutionScale, 0, GL_RGBA, GL_FLOAT, NULL);

	if (ImGuiCamera::enableLinearScaling) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width * ImGuiCamera::resolutionScale, height * ImGuiCamera::resolutionScale, 0, GL_RGBA, GL_FLOAT, NULL);

	if (ImGuiCamera::enableLinearScaling) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width * ImGuiCamera::resolutionScale, height * ImGuiCamera::resolutionScale, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	if (ImGuiCamera::enableLinearScaling) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
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
	glEnable(GL_DEPTH_TEST);
	frameBufferProgram.Activate();
	UF::Float(frameBufferProgram.ID, "time", glfwGetTime());
	UF::Float(frameBufferProgram.ID, "deltaTime", TimeUtil::s_DeltaTime);

	UF::Float(frameBufferProgram.ID, ImGuiCamera::UniformInput, ImGuiCamera::UniformFloat[0]);
	UF::Bool(frameBufferProgram.ID, "enableFB", ImGuiCamera::enableFB);

	// draw the framebuffer
	glBindVertexArray(viewVAO);
	glDisable(GL_DEPTH_TEST); // stops culling on the rectangle the framebuffer is drawn on
	glBindTexture(GL_TEXTURE_2D, frameBufferTexture);

	frameBufferProgram.Activate();
	//glActiveTexture(GL_TEXTURE3);
	//glBindTexture(GL_TEXTURE_2D, albedoTexture);
	//glUniform1i(glGetUniformLocation(frameBufferProgram.ID, "albedoMap"), 3);
	frameBufferProgram.Activate();

	// gPass textures bound to FB

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glUniform1i(glGetUniformLocation(frameBufferProgram.ID, "gPosition"), 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glUniform1i(glGetUniformLocation(frameBufferProgram.ID, "gNormal"), 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	glUniform1i(glGetUniformLocation(frameBufferProgram.ID, "gAlbedoSpec"), 3);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, depthTexture);

	if (!imGuiPanels) {

		frameBufferProgram.Activate();
		glUniform1i(glGetUniformLocation(frameBufferProgram.ID, "depthMap"), 5);
		ResizeLogic(imGuiPanels, window, Vwidth, Vheight);

		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	else{

		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, depthTexture2);
		frameBufferProgram.Activate();
		glUniform1i(glGetUniformLocation(frameBufferProgram.ID, "depthMap"), 5);

		// copy contents of FB to FB2 and Display FB2
		Framebuffer::FBO2Draw(frameBufferProgram);
		//(Shader frameBufferProgram, unsigned int& frameBufferTexture, unsigned int& frameBufferTexture2, unsigned int& FBO2)
	}

	//std::cout << "gAlbedoSpec Texture ID: " << gAlbedoSpec << std::endl;
	//std::cout << "Albedo Uniform Location: " << glGetUniformLocation(frameBufferProgram.ID, "gAlbedoSpec") << std::endl;

}
void Framebuffer::gPassDraw(Model& model, glm::vec3 Transform, glm::quat Rotation, glm::vec3 Scale) {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	gPassShader.Activate();
	UF::Float(gPassShader.ID, "gamma", RenderClass::gamma);
	glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::gBuffer);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDisable(GL_CULL_FACE);
	
	model.Draw(gPassShader, Transform, Rotation, Scale);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//FrameBuffer
	glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::FBO);
}