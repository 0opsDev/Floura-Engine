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
	static glm::vec3 CameraXYZ; // Initial camera position

	static bool RenderClass::LightingPass; // Toggle for lighting pass
	static bool RenderClass::RegularPass; // Toggle for regular pass

	static void init(GLFWwindow* window, unsigned int width, unsigned int height);

	static void Render(GLFWwindow* window, Shader frameBufferProgram, Shader shaderProgram, float window_width, float window_height, glm::vec3 lightPos,
		std::vector<std::tuple<Model, int, glm::vec3, glm::vec4, glm::vec3, int>> models);

	static void Swapchain(GLFWwindow* window, Shader frameBufferProgram, Shader shaderProgram, GLFWmonitor* primaryMonitor);

	static void Cleanup();

	static void gPassDraw(Model& model, glm::vec3 Transform, glm::vec4 Rotation, glm::vec3 Scale);

};
#endif