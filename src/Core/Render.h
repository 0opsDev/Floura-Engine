#ifndef RENDER_CLASS_H
#define RENDER_CLASS_H

#include <iostream>
#include <imgui/imgui_impl_glfw.h>
#include <camera/Camera.h>
#include <imgui/imgui_impl_opengl3.h>
#include <UI/ImGui/ImGuiWindow.h>
#include <Render/Shader/SkyBox.h>
#include <Render/Model/Model.h>
#include <Render/Cube/Billboard.h>

class RenderClass
{
public:
	static Shader shaderProgram;
	static float gamma;
	static bool doReflections;
	static bool doFog;
	static GLfloat DepthDistance;
	static GLfloat DepthPlane[];
	static GLfloat lightRGBA[];
	static GLfloat skyRGBA[];
	static GLfloat fogRGBA[];
	static GLfloat LightTransform1[];
	static GLfloat ConeSI[];
	static GLfloat ConeRot[];

	static bool DoDeferredLightingPass; // Toggle for lighting pass
	static bool DoForwardLightingPass; // Toggle for regular pass
	static bool DoComputeLightingPass;

	static void init(GLFWwindow* window, unsigned int width, unsigned int height);

	static void ClearFramebuffers();

	static void Render(GLFWwindow* window, Shader frameBufferProgram, float window_width, float window_height,
		 unsigned int width, unsigned int height);

	static void ForwardLightingPass();

	static void DeferredLightingPass();

	static void HybridLightingPass();

	static void Swapchain(GLFWwindow* window, Shader frameBufferProgram, GLFWmonitor* primaryMonitor);

	static void Cleanup();

};
#endif