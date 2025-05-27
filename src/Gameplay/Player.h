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
	//Player states
	static bool IsPlay;
	static std::string PlayerState;

	//Position stats
	static float walkSpeed;
	static float SprintSpeed;
	static float CrouchSpeed;
	static float JumpHeight;

	static bool isJumping;
	static bool isCrouching;
	static bool isSprinting;

	// Player stats
	static float Health;
	static float shield;
	static float stamina;

	// weapon stats
	struct WeaponStats {
		std::string name;
		float damage;
		float range;
		float fireRate;
		float reloadTime;
		int magazineSize;
		int currentAmmo;
		bool isReloading;
	};
	static std::string currentWeapon;
	static std::vector<WeaponStats> weaponInventory;
};

#endif