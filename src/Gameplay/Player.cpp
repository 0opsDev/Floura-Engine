#include "Player.h"
#include <Sound/SoundProgram.h>
#include <Sound/SoundRunner.h>
#include <Render/window/WindowHandler.h>
#include "Scene/scene.h"
#include <Render/Handler/RenderHandler.h>
#include "utils/FE_math.h"
bool Player::CollideWithCamera = true;
bool Player::isGrounded = false;
bool Player::isColliding = false;
bool Player::s_DoGravity = false;
bool Player::isMoving = false;
bool Player::DoJump = true;
glm::vec3 Player::velocity;
glm::vec3 Player::force;
glm::vec3 Player::gravity = glm::vec3(0.0f, -9.81f, 0.0f);
glm::vec3 Player::cameraColliderScale = glm::vec3(1.0, 1.0, 1.0);
float Player::mass = 70.0f;

glm::vec3 lastpos = glm::vec3(0.0f);

SoundProgram FootSound;
void Player::init() {

	FootSound.CreateSound("Assets/Sounds/Footsteps.wav", "FootSound");
	FootSound.Set3D(true);
}
void Player::update() {

	glm::vec3 colnorm = glm::vec3(0.0f);
	float adjustedSpeed = 5.0f * TimeUtil::deltatime;
	glm::vec3 flatOrientation = glm::normalize(glm::vec3(Scene::maincamera.Orientation.x, 0.0f, Scene::maincamera.Orientation.z));

	if (glfwGetKey(windowHandler::window, GLFW_KEY_W) == GLFW_PRESS)
	{
		Scene::maincamera.Position += adjustedSpeed * flatOrientation;
	}
	if (glfwGetKey(windowHandler::window, GLFW_KEY_A) == GLFW_PRESS)
	{
		Scene::maincamera.Position += adjustedSpeed * -glm::normalize(glm::cross(flatOrientation, Scene::maincamera.Up));
	}
	if (glfwGetKey(windowHandler::window, GLFW_KEY_S) == GLFW_PRESS)
	{
		Scene::maincamera.Position += adjustedSpeed * -flatOrientation;
	}
	if (glfwGetKey(windowHandler::window, GLFW_KEY_D) == GLFW_PRESS)
	{
		Scene::maincamera.Position += adjustedSpeed * glm::normalize(glm::cross(flatOrientation, Scene::maincamera.Up));
	}
	// very bad collisions with aabb
	float collisiondepth = 0.0f;
	if (Player::CollideWithCamera)
	{
		for (size_t i = 0; i < Scene::entityObjects.size(); i++)
		{
			int index = RenderHandler::fetchModelIndex(Scene::entityObjects[i]->component.renderHeads.renderID);
			if (index != -1)
			{
				for (size_t x = 0; x < RenderHandler::models[index].model->meshes.size(); x++)
				{
					Collision::AABB meshAABB;
					if (x < Scene::entityObjects[i]->component.collider.rootnodes.size())
					meshAABB = Scene::entityObjects[i]->component.collider.rootnodes[x];
					//Collision::AABB meshAABB = Scene::entityObjects[i]->component.collider.rootnodes[x];
					glm::vec3 globalPosition = meshAABB.position;
					glm::vec3 globalSize = FE_Math::pad(meshAABB.size, 0.4f);
					Collision::HitResult collisionData = Collision::AABBvsAABB(globalPosition, globalSize,
						glm::vec3(Scene::maincamera.Position.x, (Scene::maincamera.Position.y - (Player::cameraColliderScale.y / 2.0f)), Scene::maincamera.Position.z),
						Player::cameraColliderScale);

					// Handle collision logic

					if (collisionData.isColliding)
					{
						Scene::maincamera.Position = glm::vec3(collisionData.lastHit.x,
							(collisionData.lastHit.y + (Player::cameraColliderScale.y / 2.0f)),
							collisionData.lastHit.z);
						Player::isColliding = true;
						colnorm = collisionData.collisionNormal;
						DoJump = true;
					}

				}
			}
		}
	}
	else {
		Player::isColliding = false;
	}

	if (!s_DoGravity)
		DoJump = true;

	if (glfwGetKey(windowHandler::window, GLFW_KEY_F2) == GLFW_PRESS) {
		s_DoGravity = false;
		velocity = glm::vec3(0.0f);
		Player::CollideWithCamera = false;
	}
	if (glfwGetKey(windowHandler::window, GLFW_KEY_F3) == GLFW_PRESS) {
		s_DoGravity = true;
		Player::CollideWithCamera = true;
	}

	if (lastpos != Scene::maincamera.Position && glfwGetKey(windowHandler::window, GLFW_KEY_W) == GLFW_PRESS ||
		lastpos != Scene::maincamera.Position && glfwGetKey(windowHandler::window, GLFW_KEY_A) == GLFW_PRESS ||
		lastpos != Scene::maincamera.Position && glfwGetKey(windowHandler::window, GLFW_KEY_S) == GLFW_PRESS ||
		lastpos != Scene::maincamera.Position && glfwGetKey(windowHandler::window, GLFW_KEY_D) == GLFW_PRESS) {
		Player::isMoving = true;
	}
	else { Player::isMoving = false; }

		if (isColliding && Player::CollideWithCamera) {
			velocity = glm::vec3(0.0f);


			

			if (glfwGetKey(windowHandler::window, GLFW_KEY_SPACE) == GLFW_PRESS && DoJump) //jump
			{
				velocity += colnorm * 5.99f; // reset force at end
			}

			FootSound.SetSoundPosition(glm::vec3(Scene::maincamera.Position.x, (Scene::maincamera.Position.y - cameraColliderScale.y), Scene::maincamera.Position.z));
			FootSound.updateCameraPosition();

			if (Player::isMoving) {
				FootSound.SetVolume(SoundRunner::entityVolume);

				float minSpeed = 5.0f;  // minimum speed
				float maxSpeed = 20.0f;  // maximum speed
				float normalizedSpeed = (Scene::maincamera.speed - minSpeed) / (maxSpeed - minSpeed);

				// Clamp the value at 1.0
				normalizedSpeed = std::clamp(normalizedSpeed, 0.0f, 1.0f);
				FootSound.SetPitch(1 + normalizedSpeed);
				//std::cout << "footsound" << std::endl;
				if (!FootSound.isPlay) {
					FootSound.PlaySound();

				}
			}
			else
			{
				FootSound.StopSound();
			}

		}
		else 
		{
			FootSound.StopSound();
		}
		if (s_DoGravity) {
			force += mass * gravity; // applying foce

			velocity += force / mass * TimeUtil::deltatime;
			Scene::maincamera.Position += Player::velocity * TimeUtil::deltatime;

			force = glm::vec3(0.0f); // reset force at end
		}

		Player::DoJump = false;
		//Player::isGrounded = false;
		Player::isColliding = false;
		glm::vec3 lastpos = Scene::maincamera.Position;
}