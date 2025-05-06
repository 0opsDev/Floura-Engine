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
#include <utils/SettingsUtil.h>

class Framebuffer
{
public:

	static unsigned int ViewPortWidth, ViewPortHeight, viewVAO, viewVBO, FBO2, frameBufferTexture2, RBO2, frameBufferTexture, RBO, FBO;
	
	static void setupMainFBO(unsigned int width, unsigned int height);

	static void setupSecondFBO(unsigned int width, unsigned int height);

	static void updateFrameBufferResolution(unsigned int width, unsigned int height);

	static void FBO2Draw(Shader frameBufferProgram);

	static void FBODraw(
		Shader frameBufferProgram,
		bool imGuiPanels, unsigned int Vwidth, unsigned int Vheight, GLFWwindow* window, Camera& camera);
};

#endif
