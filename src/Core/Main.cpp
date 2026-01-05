#include "Main.h"
#include "utils/timeUtil.h" 
#include <Render/window/WindowHandler.h>
#include "Render/Object/ModelAssimp.h"
#include "utils/FE_math.h"

int main()
{

	windowHandler::width = 800;
	windowHandler::height = 600;
	Camera camera;
	camera.sensitivity = glm::vec2(100.0f);

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4), glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6); // Window Minimum and Maximum version
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //OpenGl Profile
	glfwWindowHint(GLFW_RESIZABLE, 1); // Start Resizable
	glfwWindowHint(GLFW_MAXIMIZED, 0); // Start Maximized
	glfwWindowHint(GLFW_DEPTH_BITS, 16); // DepthBuffer Bit
	glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

	windowHandler::InitMainWidnow();
	gladLoadGL(); // load open gl config

	windowHandler::setVSync(false); // Set Vsync to value of doVsync (bool)

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glFrontFace(GL_CCW);


	camera.InitCamera(windowHandler::width, windowHandler::height, glm::vec3(0.0f)); 	// camera ratio pos


	Model cubeModel("Assets/Models/lion_head_2k.gltf/lion_head_2k.gltf");
	glm::vec3 position = glm::vec3(0.0f, -3.0f, -5.0f);
	glm::vec3 scale = glm::vec3(15.0f, 15.0f, 15.0f);
	glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);

	cubeModel.gModelMatrix = FE_Math::composeMatrixWDegrees(position, scale, rotation);

	Shader regular;
	regular.LoadShader("Assets/Shaders/Lighting/Default.vert", "Assets/Shaders/Lighting/albedo.frag");

	while (!glfwWindowShouldClose(windowHandler::window)) // GAME LOOP
	{
		TimeUtil::update();
		camera.Inputs(windowHandler::window);
		camera.updateMatrix(60.0f, 0.1f, 1000.0f);

		glViewport(0, 0, windowHandler::width, windowHandler::height);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_CULL_FACE);

		cubeModel.draw(regular, camera);

		glfwSwapBuffers(windowHandler::window);
		glfwPollEvents();
	}

	glfwDestroyWindow(windowHandler::window), glfwTerminate();
	return 0;
}