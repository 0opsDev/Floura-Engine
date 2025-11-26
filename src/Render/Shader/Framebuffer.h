#ifndef FRAMEBUFFER_CLASS_H
#define FRAMEBUFFER_CLASS_H

#include<iostream>
#include<glad/glad.h>
#include<render/Shader/shaderClass.h>
#include <GLFW/glfw3.h>
#include "utils/init.h"
#include <UI/ImGui/ImGuiWindow.h>
#include <utils/ScreenUtils.h>
#include <camera/Camera.h>
#include "utils/noise.h"
#include "FramebufferObject.h"
#include "Render/Cube/RenderQuad.h"
class Framebuffer
{
public:

	static int tempWidth;
	static int tempHeight;

	static unsigned int ViewPortWidth, ViewPortHeight;
	//static unsigned int frameBufferTexture, RBO, FBO, FBO2, frameBufferTexture2;
	static GLuint noiseMapTexture;
	static Shader frameBufferProgram;
	static FramebufferObject* main; // effect
	static FramebufferObject* finalFB; // output

	static void setupFBO(unsigned int width, unsigned int height);

	static void updateFrameBufferResolution(unsigned int width, unsigned int height);

	static void FBODraw(bool imGuiPanels, GLFWwindow* window);

private:
	
	static void FBO2Draw();
	static RenderQuad rq;
};

#endif
