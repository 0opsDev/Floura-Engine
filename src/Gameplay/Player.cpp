#include "Player.h"
#include <Sound/SoundProgram.h>
#include <Sound/SoundRunner.h>
#include <algorithm>
#include <Render/window/WindowHandler.h>

bool Player::isGrounded = false;
bool Player::isColliding = false;
glm::vec3 Player::feetpos;
SoundProgram FootSound;
float JumpTimeAccumulator = 0.0f;
float PlayerPhysicsAccum = 0.0f;
float fallSpeed;
float tempcameracolYval;
void Player::init() {

	FootSound.CreateSound("Assets/Sounds/Footsteps.wav", "FootSound");
	FootSound.Set3D(true);

	tempcameracolYval = Camera::cameraColliderScale.y;
}
void Player::update() {

	if (Camera::s_DoGravity) {
			if (glfwGetKey(windowHandler::window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
				if (Camera::cameraColliderScale.y > (tempcameracolYval / 2)) {
					Camera::cameraColliderScale.y -= 5 * TimeUtil::s_DeltaTime;
				}
			}
			else {
				if (Camera::cameraColliderScale.y < tempcameracolYval) {
					Camera::cameraColliderScale.y += 5 * TimeUtil::s_DeltaTime;
				}
			}
	}

	PlayerPhysicsAccum += TimeUtil::s_DeltaTime;
	fallSpeed += 0.1f * TimeUtil::s_DeltaTime;
	if (fallSpeed > 1.0f) {
		fallSpeed = 1.0f; // cap fall speed
	}
	if (PlayerPhysicsAccum >= 0.016) {
		feetpos = glm::vec3(Camera::Position.x, (Camera::Position.y - Camera::cameraColliderScale.y), Camera::Position.z);
		FootSound.SetSoundPosition(feetpos.x, feetpos.y, feetpos.z);
		FootSound.updateCameraPosition();
		
		if (isColliding && Camera::isMoving) {
			fallSpeed = 0.0f; // reset fall speed when colliding
			FootSound.SetVolume(SoundRunner::entityVolume);

			float minSpeed = 5.0f;  // minimum speed
			float maxSpeed = 20.0f;  // maximum speed
			float normalizedSpeed = (Camera::speed - minSpeed) / (maxSpeed - minSpeed);

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
			//Camera::DoJump = false;
		}
		if (Camera::s_DoGravity && !isColliding) {
			Camera::Position.y -= fallSpeed;
			//std::cout << TimeUtil::s_DeltaTime << std::endl;
		}
		PlayerPhysicsAccum = 0.0f;

		//Player::isGrounded = false;
		Player::isColliding = false;
	}

}