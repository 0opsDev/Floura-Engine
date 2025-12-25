#include "tempscene.h"
#include "Core/Render.h"
#include "camera/Camera.h"
#include "utils/FE_math.h"
#include "Render/window/WindowHandler.h"
#include <Scene/Object/Entity.h>
#include "Scene/scene.h"
//#include "Scene/Object/Entity.h"


glm::vec3 corners[8] = {
	{-1.0f, -1.0f,  1.0f}, // 0
	{ 1.0f, -1.0f,  1.0f}, // 1
	{ 1.0f, -1.0f, -1.0f}, // 2
	{-1.0f, -1.0f, -1.0f}, // 3
	{-1.0f,  1.0f,  1.0f}, // 4
	{ 1.0f,  1.0f,  1.0f}, // 5
	{ 1.0f,  1.0f, -1.0f}, // 6
	{-1.0f,  1.0f, -1.0f}  // 7
};


glm::vec3 cubeTriangles[36] = {
	// Front face
	corners[0], corners[1], corners[5],    corners[0], corners[5], corners[4],
	// Back face
	corners[3], corners[7], corners[6],    corners[3], corners[6], corners[2],
	// Top face
	corners[4], corners[5], corners[6],    corners[4], corners[6], corners[7],
	// Bottom face
	corners[0], corners[2], corners[1],    corners[0], corners[3], corners[2],
	// Right face
	corners[1], corners[2], corners[6],    corners[1], corners[6], corners[5],
	// Left face
	corners[0], corners[4], corners[7],    corners[0], corners[7], corners[3]
};

void TempScene::init(){
}

void renderTri(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 colour)
{
	RenderClass::line->updateVBO(a, b);
	RenderClass::line->draw(colour);
	RenderClass::line->updateVBO(b, c);
	RenderClass::line->draw(colour);
	RenderClass::line->updateVBO(a, c);
	RenderClass::line->draw(colour);
}
glm::vec3 position = glm::vec3(0.0f, 0.0f, 2.0f);
void TempScene::Update() {
	/*
	if (glfwGetKey(windowHandler::window, GLFW_KEY_I) == GLFW_PRESS)
		position.z += 1.0 * TimeUtil::s_DeltaTime;
	if (glfwGetKey(windowHandler::window, GLFW_KEY_K) == GLFW_PRESS)
		position.z -= 1.0 * TimeUtil::s_DeltaTime;
	if (glfwGetKey(windowHandler::window, GLFW_KEY_J) == GLFW_PRESS)
		position.x += 1.0 * TimeUtil::s_DeltaTime;
	if (glfwGetKey(windowHandler::window, GLFW_KEY_L) == GLFW_PRESS)
		position.x -= 1.0 * TimeUtil::s_DeltaTime;
	if (glfwGetKey(windowHandler::window, GLFW_KEY_N) == GLFW_PRESS)
		position.y += 1.0 * TimeUtil::s_DeltaTime;
	if (glfwGetKey(windowHandler::window, GLFW_KEY_M) == GLFW_PRESS)
		position.y -= 1.0 * TimeUtil::s_DeltaTime;

	glm::vec3 a = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 b = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 c = glm::vec3(0.0f, 0.0f, 1.0f);

	for (size_t i = 0; i < 36; i += 3)
	{
		glm::vec3 v0 = cubeTriangles[i] + position;
		glm::vec3 v1 = cubeTriangles[i + 1] + position;
		glm::vec3 v2 = cubeTriangles[i + 2] + position;


		Collision::HitResult hr = Collision::TrianglevsTriangle(a, b, c, v0, v1, v2);

		if (hr.isColliding)
		{
			if (i == 0 ) renderTri(a, b, c, glm::vec3(1.0f, 0.0f, 0.0f));
			renderTri(v0, v1, v2, glm::vec3(1.0f, 0.0f, 0.0f));
		}
		else
		{
			if (i == 0) renderTri(a, b, c, glm::vec3(1.0f));
			renderTri(v0, v1, v2, glm::vec3(1.0f));
		}
	}




	for (size_t i = 0; i < Scene::entityObjects.size(); i++)
	{
		if (glfwGetMouseButton(windowHandler::window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		{
			glm::vec3 rayPos = Camera::Position;
			glm::vec3 rayDir = Camera::Orientation;


			Collision::HitResult result = Scene::entityObjects[i]->RayVsTriangle(rayPos, rayDir);

			if (result.isColliding)
			{
				RenderClass::line->updateVBO(rayPos + glm::vec3(0.0f, 1.0f, 0.0f), result.lastHit);
				RenderClass::line->draw(glm::vec3(0.0f, 0.0f, 1.0f));
			}
		}
	}
	*/
}

void TempScene::Delete()
{

}