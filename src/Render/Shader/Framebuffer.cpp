#include "Render/Shader/Framebuffer.h"
#include <Render/passes/geometry/geometryPass.h>
#include <Render/passes/lighting/LightingPass.h>
#include <Render/window/WindowHandler.h>
#include <utils/logConsole.h>
#include <Scene/LightingHandler.h>

int Framebuffer::tempWidth;
int Framebuffer::tempHeight;

unsigned int Framebuffer::ViewPortWidth = 800, Framebuffer::ViewPortHeight = 600;

//unsigned int Framebuffer::viewVAO, Framebuffer::viewVBO;
//unsigned int Framebuffer::FBO2;

//unsigned int Framebuffer::frameBufferTexture2; //Framebuffer::frameBufferTexture;
//unsigned int Framebuffer::RBO, Framebuffer::FBO;

GLuint Framebuffer::noiseMapTexture;
Shader Framebuffer::frameBufferProgram;

FramebufferObject *Framebuffer::main;
FramebufferObject *Framebuffer::finalFB;
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

	main = new FramebufferObject(glm::vec2(width, height), true);
	finalFB = new FramebufferObject(glm::vec2(width, height), false);
}
void Framebuffer::updateFrameBufferResolution(unsigned int width, unsigned int height) {
	Framebuffer::ViewPortWidth = width;
	Framebuffer::ViewPortHeight = height;

	main->resizeResolution(glm::vec2(width, height));
	finalFB->resizeResolution(glm::vec2(width, height));

	GeometryPass::updateGbufferResolution(width, height);

	LightingPass::resizeTexture(width, height);
}

void Framebuffer::FBO2Draw() {
	// Apply post-processing and render to the second FBO
	glBindFramebuffer(GL_FRAMEBUFFER, finalFB->FBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	frameBufferProgram.Activate();
	glDisable(GL_DEPTH_TEST);
	glBindTexture(GL_TEXTURE_2D, main->texture);
	// here
	glDrawArrays(GL_TRIANGLES, 0, 6);
	rq.draw(frameBufferProgram);
	// Copy the contents of the second FBO to the default FBO
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	frameBufferProgram.Activate();
	glDisable(GL_DEPTH_TEST);
	glBindTexture(GL_TEXTURE_2D, finalFB->texture);
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
	glBindTexture(GL_TEXTURE_2D, main->texture);

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

	frameBufferProgram.setFloat("gamma", RenderClass::gamma);

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