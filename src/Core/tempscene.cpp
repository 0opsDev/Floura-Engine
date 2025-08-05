#include "tempscene.h"
#include <Sound/SoundProgram.h>
#include <Sound/SoundRunner.h>
#include <Gameplay/FE_Object.h>
#include <Render/window/WindowHandler.h>

//BillBoardObject BBOJ2;
//BillBoardObject pot;
//CubeCollider flatplane;
//ModelObject test2;
//ModelObject plane;
//SoundProgram Soundtrack;

//FE_Object PlaceHolder;

void TempScene::init(){

	//PlaceHolder.Create("model", "PlaceHolder", "Assets/Models/barrel/barrel2.gltf", "Assets/Material/Default.Material");
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

	//PlaceHolder.UpdateLights();
	//PlaceHolder.Draw();

}

void TempScene::Delete() {
}