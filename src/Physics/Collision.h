#include <glm/ext/vector_float3.hpp>
#ifndef COLLISION_H
#define COLLISION_H

/*
	static class for handling collision detection between different collider types
	returns true if a collision is detected, false otherwise
	also returns collision data such as collision point, normal, etc.
	which will be returned in a form of a struct? 
*/

class Collision
{
public:

	static struct collisionData {
		bool isColliding = false;
		glm::vec3 lastHit = glm::vec3(0.0f);
		glm::vec3 collisionNormal = glm::vec3(0.0f);
	};

	static struct AABB
	{
		glm::vec3 position = glm::vec3(0.0f);
		glm::vec3 size = glm::vec3(0.0f);
	};

	// AABB collision detection
	// AABB vs AABB
	static collisionData AABBvsAABB(
		const glm::vec3& posA, const glm::vec3& sizeA,
		const glm::vec3& posB, const glm::vec3& SizeB);

	// Triangle vs Triangle
	static collisionData TrianglevsTriangle(
		const glm::vec3& v0A, const glm::vec3& v1A, const glm::vec3& v2A,
		const glm::vec3& v0B, const glm::vec3& v1B, const glm::vec3& v2B);


private:

};

#endif