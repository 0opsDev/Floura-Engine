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

	static bool isGrounded;
	static bool isColliding;

	static void init();

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