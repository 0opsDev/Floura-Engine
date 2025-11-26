#include "tempscene.h"
#include <Sound/SoundProgram.h>
#include <Sound/SoundRunner.h>
#include <Scene/Object/Entity.h>
#include <Render/window/WindowHandler.h>
#include "IdManager.h"
#include <Render/Object/ModelAssimp.h>
#include "Scene/scene.h"
#include "Render/Object/line.h"
#include "Physics/Collision.h"
#include "Render/Shader/Material.h"

Model* sphere;
Material mat;

void TempScene::init(){
	sphere = new Model("Assets/Models/basic shapes/sphere.gltf");
	mat.LoadMaterial("Assets/Material/DefaultNoNormals.Material");
}

void drawSphere(glm::vec3 pos)
{


	mat.update();
	mat.updateForwardLights();

	mat.ModelShader.Activate();
	mat.ModelShader.setFloat2("uvScale", glm::vec2(1.0f));
	mat.ModelGpassShader.Activate();
	mat.ModelGpassShader.setFloat2("uvScale", glm::vec2(1.0f));

	sphere->updatePosition(pos);
	sphere->updateRotation(glm::vec3(0.0f));
	sphere->updateScale(glm::vec3(1.0f));
	glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::main->FBO);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	sphere->draw(mat.ModelShader);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TempScene::Update() {

	if (glfwGetKey(windowHandler::window, GLFW_KEY_F2) == GLFW_PRESS) {
		Camera::s_DoGravity = false;
		CubeCollider::CollideWithCamera = false;
	}
	if (glfwGetKey(windowHandler::window, GLFW_KEY_F3) == GLFW_PRESS) {
		Camera::s_DoGravity = true;
		CubeCollider::CollideWithCamera = true;
	}
	/*
		glm::vec3 p1 = glm::vec3(-3.0f, 3.0f, 0.0f);
	glm::vec3 p2 = glm::vec3(-4.2f, 4.1f, 1.2f);
	drawSphere(p1);
	drawSphere(p2);
	
	Collision::collisionData data = Collision::SpherevsSphere(p1, p2, 1.0f, 1.0f);
	if (data.isColliding)
	{
		std::cout << "colliding" << std::endl;
	}
	*/
}

void TempScene::Delete() {
	sphere->~Model();
}