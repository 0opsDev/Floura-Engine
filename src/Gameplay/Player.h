#ifndef PLAYER_CLASS_H
#define PLAYER_CLASS_H

#include "camera/Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <string>
class Player
{
public:


	static bool CollideWithCamera;
	static bool isGrounded;
	static bool isColliding;

	static void init();
	// this is not meant for a player controller, im only placing it here for now

	// 0 stop
	// 1 play
	// 2 pause
	static int playstate;

	static void stopState();
	static void playState();
	static void pauseState();

	static void update();

	static glm::vec3 cameraColliderScale;
	static bool DoJump;
	static bool s_DoGravity;
	static bool isMoving;
private:
	static glm::vec3 velocity;
	static glm::vec3 force;
	static glm::vec3 gravity;
	static float mass;

};

#endif