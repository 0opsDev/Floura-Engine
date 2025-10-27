#ifndef RENDER_CLASS_H
#define RENDER_CLASS_H

#include <iostream>
#include <camera/Camera.h>
#include <UI/ImGui/ImGuiWindow.h>
#include <Render/Shader/SkyBox.h>
#include <Render/Cube/Billboard.h>

class RenderClass
{
public:

	// billboard
	static Shader billBoardShader;
	static Shader gPassShaderBillBoard;
	static Shader boxShader;
	static float gamma;
	static bool renderSkybox;
	static bool doReflections;
	static bool doFog;
	static GLfloat DepthDistance;
	static GLfloat DepthPlane[];
	static glm::vec3 skyRGBA;
	static glm::vec3 fogRGBA;

	static bool DoDeferredLightingPass; // Toggle for lighting pass
	static bool DoForwardLightingPass; // Toggle for regular pass
	static bool DoComputeLightingPass;

	static void init(unsigned int width, unsigned int height);

	static void initGlobalShaders();

	static void ClearFramebuffers();

	static void Render(GLFWwindow* window, unsigned int width, unsigned int height);

	static void ForwardLightingPass();

	static void DeferredLightingPass();

	static void HybridLightingPass();

	static void Swapchain(GLFWwindow* window);

	static void Cleanup();

};
#endif