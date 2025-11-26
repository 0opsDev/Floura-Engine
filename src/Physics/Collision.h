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

	static struct HitResult {
		bool isColliding = false;
		glm::vec3 lastHit = glm::vec3(0.0f);
		glm::vec3 collisionNormal = glm::vec3(0.0f);
	};

	static struct AABB
	{
		glm::vec3 position = glm::vec3(0.0f);
		glm::vec3 size = glm::vec3(0.0f);
	};

	static struct Sphere
	{
		glm::vec3 position = glm::vec3(0.0f);
		float radius = 1.0f;
	};

	static struct collisionObject
	{
		AABB aabb;
		Sphere sphere;
		char type; // A = AABB, S = sphere
	};

	// AABB collision detection
	// AABB vs AABB
	static HitResult AABBvsAABB(
		const glm::vec3& posA, const glm::vec3& sizeA,
		const glm::vec3& posB, const glm::vec3& SizeB);

	// Triangle vs Triangle
	static HitResult TrianglevsTriangle(
		const glm::vec3& v0A, const glm::vec3& v1A, const glm::vec3& v2A,
		const glm::vec3& v0B, const glm::vec3& v1B, const glm::vec3& v2B);

	// sphere vs sphere
	static HitResult SpherevsSphere(
		const glm::vec3& P1, const glm::vec3& P2,
		float R1, float R2); // two points and radius


private:

};

#endif