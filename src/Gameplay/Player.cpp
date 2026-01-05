#include "Player.h"
#include <Sound/SoundProgram.h>
#include <Sound/SoundRunner.h>
#include <Render/window/WindowHandler.h>
#include "Scene/scene.h"
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

		if (isColliding) {
			velocity = glm::vec3(0.0f);
			if (glfwGetKey(windowHandler::window, GLFW_KEY_SPACE) == GLFW_PRESS && DoJump) //jump
			{
				velocity.y += 5.99f; // reset force at end
				//force += glm::vec3(0.0f, 2800.0f, 0.0f) * mass;
				//826.7
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

		//Player::isGrounded = false;
		Player::isColliding = false;
		glm::vec3 lastpos = Scene::maincamera.Position;
}