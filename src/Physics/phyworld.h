#ifndef PHYS_WORLD_H
#define PHYS_WORLD_H

#include "Collision.h"
#include <vector>

class PhysWorld
{
public:

	static struct CollisionResponse
	{
	
	};

	static struct OngoingCollision
	{

	};

	static struct SettingsObject
	{
		bool hasCollider = false;
		bool hasPhysics = false;
		bool affectedByGravity = false;
		bool isCollidable = false;
	};

	static struct physicsObject
	{
		SettingsObject setting;
		Collision::collisionObject collisionObject; 
		Collision::HitResult detection;
		CollisionResponse response;
		OngoingCollision collision;
	};
	static std::vector <physicsObject> physicsObjects;

	static void begintick(std::vector <physicsObject> nPhysicsObjects); // onload physics objects for tick

	static void simulate(); // should take in phys object

	float tickrate = 30.0f; // could do 60

private:

	static void draw(); // debug draw

};

#endif