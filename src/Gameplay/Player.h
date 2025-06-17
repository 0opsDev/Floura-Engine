#ifndef PLAYER_CLASS_H
#define PLAYER_CLASS_H

#include "camera/Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <string>
#include <vector>
class Player
{
public:

	static bool isGrounded;
	static bool isColliding;

	static glm::vec3 feetpos;

	static void init();

	static void update();

};

#endif