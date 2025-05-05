#ifndef FRAMEBUFFER_CLASS_H
#define FRAMEBUFFER_CLASS_H

#include<iostream>
#include<glad/glad.h>
#include<render/Shader/shaderClass.h>
#include <GLFW/glfw3.h>
#include "utils/init.h"
#include <UI/ImGui/ImGuiWindow.h>
#include <utils/UF.h>
#include <utils/ScreenUtils.h>
#include <camera/Camera.h>

class Framebuffer
{
public:

	static unsigned int ViewPortWidth;
	static unsigned int ViewPortHeight;
	
	void setupMainFBO(unsigned int& viewVAO, unsigned int& viewVBO, unsigned int& FBO, unsigned int& frameBufferTexture, unsigned int& RBO, unsigned int width, unsigned int height, const float* ViewportVerticies);

	void setupSecondFBO(unsigned int& FBO, unsigned int& frameBufferTexture, unsigned int& RBO, unsigned int width, unsigned int height);

	static void updateFrameBufferResolution(unsigned int& frameBufferTexture, unsigned int& RBO, unsigned int& frameBufferTexture2, unsigned int& RBO2, unsigned int width, unsigned int height);

	static void Framebuffer::FBO2Draw(Shader frameBufferProgram, unsigned int& frameBufferTexture, unsigned int& viewVAO, unsigned int& frameBufferTexture2, unsigned int& FBO2);

	static void FBODraw(
		Shader frameBufferProgram, unsigned int& frameBufferTexture, unsigned int& viewVAO,
		unsigned int& frameBufferTexture2, unsigned int& FBO2, unsigned int& RBO, unsigned int& RBO2,
		bool imGuiPanels, unsigned int Vwidth, unsigned int Vheight, GLFWwindow* window, Camera& camera);
};

#endif
