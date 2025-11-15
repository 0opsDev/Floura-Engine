#include "tempscene.h"
#include <Sound/SoundProgram.h>
#include <Sound/SoundRunner.h>
#include <Scene/Object/Entity.h>
#include <Render/window/WindowHandler.h>
#include "IdManager.h"
#include <Render/Object/ModelAssimp.h>
#include "Scene/scene.h"
#include "Render/Object/line.h"

void TempScene::init(){
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
}

void TempScene::Delete() {
}