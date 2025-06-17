#include "scene.h"
#include <Sound/SoundProgram.h>
#include <Sound/SoundRunner.h>

//BillBoardObject BBOJ2;
BillBoardObject BBOJ;
CubeCollider flatplane;
ModelObject test2;
SoundProgram Soundtrack;
SoundProgram Wind;

void Scene::init(){

	Wind.CreateSound("Assets/Sounds/Cold wind sound effect 4.wav");
	Soundtrack.CreateSound("Assets/Sounds/Clair de Lune.wav");

	//BBOJ2.CreateObject("Animated", "Assets/Sprites/animatedBillboards/fire/fire.json", "fire");
	//BBOJ2.tickrate = 20;
	//BBOJ2.doPitch = true;
	//BBOJ2.transform = glm::vec3(5, 5, 5);
	//BBOJ2.scale = glm::vec3(1, 1, 1);
	//BBOJ2.isCollider = true;
	
	//BBOJ.CreateObject("Static", "Assets/Sprites/pot.png", "pot");
	//BBOJ.doPitch = false;
	//BBOJ.transform = glm::vec3(-3, 0, 1.8);
	//BBOJ.scale = glm::vec3(0.5, 0.5, 0.5);
	//BBOJ.isCollider = true;

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
	// sound

	if (!Soundtrack.isPlay) { Soundtrack.PlaySound(SoundRunner::MusicVolume); }
	Soundtrack.SetVolume(SoundRunner::MusicVolume);
	Soundtrack.updateCameraPosition();
	//Soundtrack.SetSoundPosition(2, 1, 0);
	Soundtrack.SetSoundPosition(Camera::Position.x, Camera::Position.y, Camera::Position.z);

	if (!Wind.isPlay) { Wind.PlaySound(SoundRunner::environmentVolume); }
	Wind.SetVolume(SoundRunner::environmentVolume);
	Wind.updateCameraPosition();
	Wind.SetSoundPosition(0, 1, 0);
	//Wind.SetSoundPosition(Camera::Position.x, Camera::Position.y, Camera::Position.z);
	
	//BBOJ2.UpdateCollider();
	//BBOJ2.UpdateCameraCollider();
	//BBOJ2.draw();

	//BBOJ.UpdateCollider();
	//BBOJ.UpdateCameraCollider();
	//BBOJ.draw();

	flatplane.update();
	flatplane.draw();

	test2.UpdateCollider();
	test2.UpdateCameraCollider();

	test2.draw(RenderClass::shaderProgram);

}

void Scene::Delete() {

	Soundtrack.DeleteSound();
}