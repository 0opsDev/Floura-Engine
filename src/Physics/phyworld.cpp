#include "Physics/phyworld.h"


std::vector <PhysWorld::physicsObject> PhysWorld::physicsObjects;

void PhysWorld::begintick(std::vector <PhysWorld::physicsObject> nPhysicsObjects)
{
	PhysWorld::physicsObjects.clear();
	PhysWorld::physicsObjects = nPhysicsObjects;
}// onload physics objects for tick

void PhysWorld::simulate()
{

	// #1 Detection
	
	// #2 response

	// #3 ongoing

}

void PhysWorld::draw()
{

}