#ifndef RENDER_CLASS_H
#define RENDER_CLASS_H

#include <iostream>
#include <imgui/imgui_impl_glfw.h>
#include <camera/Camera.h>
#include <imgui/imgui_impl_opengl3.h>
#include <UI/ImGui/ImGuiWindow.h>
#include <Render/Shader/SkyBox.h>
#include <Render/Model/Model.h>

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

	static void init(GLFWwindow* window, unsigned int width, unsigned int height);

	static void Render(GLFWwindow* window, Shader frameBufferProgram, Shader shaderProgram, Shader LightProgram, Shader SolidColour, float window_width, float window_height, glm::vec3 lightPos, Model Lightmodel,
		std::vector<std::tuple<Model, int, glm::vec3, glm::quat, glm::vec3, int>> models);

	static void Swapchain(GLFWwindow* window, Shader frameBufferProgram, Shader shaderProgram, GLFWmonitor* primaryMonitor);

	static void Cleanup();
};
#endif