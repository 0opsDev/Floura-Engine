#include "Collision.h"
#include <glm/common.hpp>
#include <algorithm>

Collision::collisionData Collision::AABBvsAABB(
	const glm::vec3& posA, const glm::vec3& sizeA,
	const glm::vec3& posB, const glm::vec3& sizeB)
{
	collisionData data;

	//data.collisionNormal;
	//data.lastHit;

	// Calculate min and max points for both AABBs
	glm::vec3 AMin = posA - sizeA; // col 
	glm::vec3 AMax = posA + sizeA;
	glm::vec3 BMin = posB - sizeB * 0.5f; // vic
	glm::vec3 BMax = posB + sizeB * 0.5f;

	// Check for overlap on each axis

	bool overlapX = (BMax.x >= AMin.x) && (BMin.x <= AMax.x);
	bool overlapY = (BMax.y >= AMin.y) && (BMin.y <= AMax.y);
	bool overlapZ = (BMax.z >= AMin.z) && (BMin.z <= AMax.z);

	// If there's overlap on all axes, we have a collision
	if (overlapX && overlapY && overlapZ) 
	{
		// set if the AABBs are colliding
		data.isColliding = true;
		// calculate the collision point

		// Determine the closest face and push the victim fully outside
		float leftDist = abs(BMax.x - AMin.x);
		float rightDist = abs(BMin.x - AMax.x);
		float bottomDist = abs(BMax.y - AMin.y);
		float topDist = abs(BMin.y - AMax.y);
		float frontDist = abs(BMax.z - AMin.z);
		float backDist = abs(BMin.z - AMax.z);

		// Find the minimum distance
		float minDist = std::min({ leftDist, rightDist, bottomDist, topDist, frontDist, backDist });

		if (minDist == leftDist) {
			data.collisionNormal = glm::vec3(-1.0f, 0.0f, 0.0f);
			data.lastHit = glm::vec3(AMin.x - sizeB.x * 0.5f, posB.y, posB.z);
		}
		else if (minDist == rightDist) {
			data.collisionNormal = glm::vec3(1.0f, 0.0f, 0.0f);
			data.lastHit = glm::vec3(AMax.x + sizeB.x * 0.5f, posB.y, posB.z);
		}
		else if (minDist == bottomDist) {
			data.collisionNormal = glm::vec3(0.0f, -1.0f, 0.0f);
			data.lastHit = glm::vec3(posB.x, AMin.y - sizeB.y * 0.5f, posB.z);
		}
		else if (minDist == topDist) {
			data.collisionNormal = glm::vec3(0.0f, 1.0f, 0.0f);
			data.lastHit = glm::vec3(posB.x, AMax.y + sizeB.y * 0.5f, posB.z);
		}
		else if (minDist == frontDist) {
			data.collisionNormal = glm::vec3(0.0f, 0.0f, -1.0f);
			data.lastHit = glm::vec3(posB.x, posB.y, AMin.z - sizeB.z * 0.5f);
		}
		else if (minDist == backDist) {
			data.collisionNormal = glm::vec3(0.0f, 0.0f, 1.0f);
			data.lastHit = glm::vec3(posB.x, posB.y, AMax.z + sizeB.z * 0.5f);
		}
	}

	return data;
}

Collision::collisionData Collision::TrianglevsTriangle(
	const glm::vec3& v0A, const glm::vec3& v1A, const glm::vec3& v2A,
	const glm::vec3& v0B, const glm::vec3& v1B, const glm::vec3& v2B)
{
	collisionData data;
	// Placeholder implementation
	data.isColliding = false;
	data.lastHit = glm::vec3(0.0f);
	data.collisionNormal = glm::vec3(0.0f);
	// Actual triangle-triangle collision detection logic would go here


	return data;
}