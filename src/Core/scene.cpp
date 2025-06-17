#include "scene.h"

//BillBoardObject BBOJ2;
BillBoardObject BBOJ;
CubeCollider flatplane;
ModelObject test2;


void Scene::init(){
	//BBOJ2.CreateObject("Animated", "Assets/Sprites/animatedBillboards/fire/fire.json", "fire");
	//BBOJ2.tickrate = 20;
	//BBOJ2.doPitch = true;
	//BBOJ2.transform = glm::vec3(5, 5, 5);
	//BBOJ2.scale = glm::vec3(1, 1, 1);
	//BBOJ2.isCollider = true;
	BBOJ.CreateObject("Static", "Assets/Sprites/pot.png", "pot");
	BBOJ.doPitch = false;
	BBOJ.transform = glm::vec3(-3, 0, 1.8);
	BBOJ.scale = glm::vec3(0.5, 0.5, 0.5);
	BBOJ.isCollider = true;

	flatplane.init();
	flatplane.colliderScale = glm::vec3(100, 1, 100); // Set collider scale for flat plane
	flatplane.colliderXYZ = glm::vec3(0, -1, 0); // Set collider transform for flat plane
	flatplane.CollideWithCamera = true;

	test2.CreateObject("LOD", "Assets/LodModel/Vase/VaseLod.json", "test2");
	test2.transform = glm::vec3(5, 0.65, 3);
	test2.scale = glm::vec3(0.3, 0.3, 0.3);
	test2.rotation = glm::vec4(0, 0, 0, 1);
	test2.isCollider = true;
	test2.DoFrustumCull = true;
	test2.BoxColliderScale = glm::vec3(0.3, 0.65, 0.3);
	test2.frustumBoxTransform = test2.BoxColliderTransform;
	test2.frustumBoxScale = test2.BoxColliderScale;
}

void Scene::Update() {
	//BBOJ2.UpdateCollider();
	//BBOJ2.UpdateCameraCollider();
	//BBOJ2.draw();

	BBOJ.UpdateCollider();
	BBOJ.UpdateCameraCollider();
	BBOJ.draw();

	flatplane.update();
	flatplane.draw();

	test2.UpdateCollider();
	test2.UpdateCameraCollider();

	test2.draw(RenderClass::shaderProgram);
}