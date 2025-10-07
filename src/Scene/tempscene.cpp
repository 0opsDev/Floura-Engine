#include "tempscene.h"
#include <Sound/SoundProgram.h>
#include <Sound/SoundRunner.h>
#include <Scene/Object/FE_Object.h>
#include <Render/window/WindowHandler.h>
#include "IdManager.h"
//FE_Object PlaceHolder;

void TempScene::init(){
	//for (size_t i = 0; i < 255; i++) {
	//	IdManager::AddID('m', i);
	//}
	//PlaceHolder.create("model", "PlaceHolder", "Assets/Models/cube.gltf", "Assets/Material/Default.Material");
	//PlaceHolder.gPosition = glm::vec3(5.0f, 0.0f, 0.0f);
	//PlaceHolder.gScale = glm::vec3(1.0f, 3.0f, 1.0f);
	//PlaceHolder.gRotation = glm::vec3(45.0f, 0.0f, 45.0f);
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

	//PlaceHolder.updateLights();
	//PlaceHolder.update();
	//PlaceHolder.draw();

}

void TempScene::Delete() {
}