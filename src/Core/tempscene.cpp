#include "tempscene.h"
#include <Sound/SoundProgram.h>
#include <Sound/SoundRunner.h>

//BillBoardObject BBOJ2;
//BillBoardObject pot;
//CubeCollider flatplane;
//ModelObject test2;
//ModelObject plane;
//SoundProgram Soundtrack;

void TempScene::init(){

	//Soundtrack.CreateSound("Assets/Sounds/brucedMono.wav");

	//BBOJ2.CreateObject("Animated", "Assets/Sprites/animatedBillboards/fire/fire.json", "fire");
	//BBOJ2.tickrate = 20;
	//BBOJ2.doPitch = true;
	//BBOJ2.transform = glm::vec3(5, 5, 5);
	// BBOJ2.scale = glm::vec3(1, 1, 1);
	//BBOJ2.isCollider = true;
	
	//pot.CreateObject("Static", "Assets/Sprites/pot.png", "pot");
	//pot.doPitch = false;
	//pot.transform = glm::vec3(-3, 0, 1.8);
	//pot.scale = glm::vec3(0.5, 0.5, 0.5);
	//pot.isCollider = true;

	//flatplane.init();
	//flatplane.colliderScale = glm::vec3(15, 1, 15); // Set collider scale for flat plane
	//flatplane.colliderXYZ = glm::vec3(0, -1, 0); // Set collider transform for flat plane
	//flatplane.CollideWithCamera = true;


	//plane.CreateObject("STATIC", "Assets/Models/Baseplate/plate.gltf", "plate");
	//plane.transform = glm::vec3(0, 0, 0);
	//plane.scale = glm::vec3(4, 0.1, 4);
	//plane.rotation = glm::vec4(0, 0, 0, 0);
	//plane.isCollider = true;
	//plane.DoFrustumCull = false;
	//plane.BoxColliderTransform = glm::vec3(0, -0.5, 0);
	//plane.BoxColliderScale = glm::vec3(4, 0.5, 4);
	//plane.frustumBoxTransform = plane.BoxColliderTransform;
	//plane.frustumBoxScale = plane.BoxColliderScale;

	//test2.CreateObject("LOD", "Assets/LodModel/Vase/VaseLod.json", "test2");
	//test2.transform = glm::vec3(5, 0.65, 3);
	//test2.scale = glm::vec3(0.3, 0.3, 0.3);
	//test2.rotation = glm::vec4(0, 0, 0, 1);
	//test2.isCollider = true;
	//test2.DoFrustumCull = true;
	//test2.BoxColliderScale = glm::vec3(0.3, 0.65, 0.3);
	//test2.frustumBoxTransform = test2.BoxColliderTransform;
	//test2.frustumBoxScale = test2.BoxColliderScale;
}

void TempScene::Update() {
	// sound

	//if (!Soundtrack.isPlay) { Soundtrack.PlaySound(); }
	//Soundtrack.SetVolume(SoundRunner::MusicVolume);
	//Soundtrack.updateCameraPosition();
	//Soundtrack.SetSoundPosition(2, 1, 0);
	//Soundtrack.Set3D(true);
	//Soundtrack.SetSoundPosition(10,0,0);
	
	//BBOJ2.UpdateCollider();
	//BBOJ2.UpdateCameraCollider();
	//BBOJ2.draw();

	//pot.UpdateCollider();
	//pot.UpdateCameraCollider();

	// move the pot towards the camera

	//float potFlySpeed = 1.0f; // Speed at which the pot moves towards the camera

	//x
	//if (pot.transform.x < Camera::Position.x){pot.transform.x += potFlySpeed * TimeUtil::s_DeltaTime;}
	//else if (pot.transform.x > Camera::Position.x){pot.transform.x -= potFlySpeed * TimeUtil::s_DeltaTime;}
	//y
	//if (pot.transform.y < (Camera::Position.y - 1.3)){pot.transform.y += potFlySpeed * TimeUtil::s_DeltaTime;}
	//else if (pot.transform.y > (Camera::Position.y - 1.3)){pot.transform.y -= potFlySpeed * TimeUtil::s_DeltaTime;}
	// z
	//if (pot.transform.z < Camera::Position.z){pot.transform.z += potFlySpeed * TimeUtil::s_DeltaTime;}
	//else if (pot.transform.z > Camera::Position.z){pot.transform.z -= potFlySpeed * TimeUtil::s_DeltaTime;}

	//pot.draw();

	//plane.UpdateCollider();
	//plane.UpdateCameraCollider();
	//plane.draw(RenderClass::shaderProgram);

	//test2.UpdateCollider();
	//test2.UpdateCameraCollider();

	//test2.draw(RenderClass::shaderProgram);

}

void TempScene::Delete() {
}