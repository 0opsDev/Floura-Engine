#include "Player.h"
#include <Sound/SoundProgram.h>
#include <Sound/SoundRunner.h>
#include <algorithm>

bool Player::isGrounded = false;
bool Player::isColliding = false;
glm::vec3 Player::feetpos;
SoundProgram FootSound;
float JumpTimeAccumulator = 0.0f;
float PlayerPhysicsAccum = 0.0f;
void Player::init() {

	FootSound.CreateSound("Assets/Sounds/Footsteps.wav", "FootSound");
	FootSound.Set3D(true);
}
void Player::update() {

	PlayerPhysicsAccum += TimeUtil::s_DeltaTime;
	if (PlayerPhysicsAccum >= 0.016) {
		feetpos = glm::vec3(Camera::Position.x, (Camera::Position.y - Camera::PlayerHeightCurrent), Camera::Position.z);
		FootSound.SetSoundPosition(feetpos.x, feetpos.y, feetpos.z);
		FootSound.updateCameraPosition();
		
		if (isColliding && Camera::isMoving) {
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
			Camera::Position.y -= (0.3);
			//std::cout << TimeUtil::s_DeltaTime << std::endl;
		}


		//if (isColliding && Camera::s_DoGravity) {
		//	JumpTimeAccumulator = 0.0f; // reset JumpTimeAccumulator
		//}

		//if (Camera::DoJump) {Camera::Position.y += 5 * TimeUtil::s_DeltaTime;}
		//else {Camera::Position.y -= 5 * TimeUtil::s_DeltaTime;}
		/*
		do jump true = jump up
		do jump false = fall down
		*/

		//if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		//	if (Camera::s_DoGravity) { // no col checks for now
			//	JumpTimeAccumulator += TimeUtil::s_DeltaTime;

				//if (JumpTimeAccumulator <= 0.3 && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
				//	Camera::DoJump = true;
					// jump logic 
				//}
				//if (JumpTimeAccumulator <= 0.3 && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
				//	Camera::DoJump = false;
				//}
				//if (JumpTimeAccumulator > 0.3) {
				//	Camera::DoJump = false; // stop jumping after 0.3 seconds
				//}

				//std::cout << JumpTimeAccumulator << std::endl;
				//std::cout << Camera::DoJump << std::endl;
			//}
		//}
		PlayerPhysicsAccum = 0.0f;
	}

}