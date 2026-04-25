#ifndef FRAMEBUFFER_CLASS_H
#define FRAMEBUFFER_CLASS_H

#include<iostream>
#include <glad/gl.h>
#include<render/Shader/shaderClass.h>
#include <GLFW/glfw3.h>
#include <Editor/UI/ImGui/ImGuiWindow.h>
#include <utils/ScreenUtils.h>
#include <camera/Camera.h>
#include "FramebufferObject.h"
#include "Render/Object/RenderQuad.h"

class Framebuffer
{
public:

	static int tempWidth;
	static int tempHeight;

	static unsigned int ViewPortWidth, ViewPortHeight;
	//static unsigned int frameBufferTexture, RBO, FBO, FBO2, frameBufferTexture2;
	static Shader frameBufferProgram;
	static unsigned int FBO, RBO, screentexture;
	static unsigned int FFBO, FRBO, Ftexture;
	static unsigned int cmFBO, cmRBO, cmtexture;
	
	static float sharpness;

	static void smInit(glm::vec2 res);

	static void smUpdateResolution(glm::vec2 res);

	static void clearsmbuffer();

	static void setupFBO(unsigned int width, unsigned int height);

	static void updateFrameBufferResolution(unsigned int width, unsigned int height);

	static void FBODraw(bool imGuiPanels, GLFWwindow* window);

	static void Delete();

private:
	
	static void FBO2Draw();
	static RenderQuad rq;
};

#endif
