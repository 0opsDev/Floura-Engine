#include "tempscene.h"
#include "Core/Render.h"
#include "camera/Camera.h"
#include "utils/FE_math.h"
#include "Render/window/WindowHandler.h"
#include <Scene/Object/Entity.h>
#include "Scene/scene.h"
#include "Render/Cube/CubeVisualizer.h"

CubeVisualizer* cube;

void TempScene::init(){
	cube = new CubeVisualizer();
}

glm::vec3 position = glm::vec3(0.0f, 0.0f, 2.0f);
void TempScene::Update() {
	/*
	glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::FBO);

	float closestDist = std::numeric_limits<float>::max();
	Collision::HitResult closestResult;
	bool hitAnything = false;

	glm::vec3 rayPos = Camera::Position;
	glm::vec3 rayDir = Camera::Orientation;

	//glm::vec3 rayPos = glm::vec3(0.0f, 10.0f, 0.0f);
	//glm::vec3 rayDir = glm::vec3(0.0f, -1.0f, 0.0f);

	for (size_t i = 0; i < Scene::entityObjects.size(); i++)
	{
		if (glfwGetMouseButton(windowHandler::window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		{

			Collision::HitResult result = Scene::entityObjects[i]->RayVsTriangle(rayPos, rayDir);

			if (result.isColliding && result.distance < closestDist) {
				{
					closestDist = result.distance;
					closestResult = result;
					hitAnything = true;
					
				}
			}
		}

		if (hitAnything) {
			cube->draw(closestResult.lastHit, glm::vec3(0.1f), glm::vec3(1.0f, 0.0f, 0.0f));
			std::cout << "hit" << std::endl;
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	*/

}

void TempScene::Delete()
{
	delete cube;
	cube = nullptr;
}