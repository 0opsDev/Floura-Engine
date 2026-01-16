#include <glm/ext/vector_float3.hpp>
#include <vector>
#include <Render/Buffer/VBO.h>
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

	struct HitResult {
		bool isColliding = false;
		float distance = 0.0f;
		float depth = 0.0f;
		glm::vec3 lastHit = glm::vec3(0.0f);
		glm::vec3 collisionNormal = glm::vec3(0.0f);

	}; 

	struct AABB
	{
		glm::vec3 position = glm::vec3(0.0f);
		glm::vec3 size = glm::vec3(0.0f);
	};

	struct Sphere
	{
		glm::vec3 position = glm::vec3(0.0f);
		float radius = 1.0f;
	};

	struct collisionObject
	{
		AABB aabb;
		Sphere sphere;
		char type; // A = AABB, S = sphere
	};

	struct tri
	{
		glm::vec3 a = glm::vec3(0.0f);
		glm::vec3 b = glm::vec3(0.0f);
		glm::vec3 c = glm::vec3(0.0f);
	};


	static bool showBoxCollider;

	// AABB

	struct rubiksCubePoints
	{
		// based off rubix cube notation
		glm::vec3 ULF = glm::vec3(0.0f); // up left front
		glm::vec3 URF = glm::vec3(0.0f); // up right front
		glm::vec3 URB = glm::vec3(0.0f); // up right back
		glm::vec3 ULB = glm::vec3(0.0f); // up left back
		glm::vec3 DLF = glm::vec3(0.0f); // down left front
		glm::vec3 DRF = glm::vec3(0.0f); // down right front
		glm::vec3 DRB = glm::vec3(0.0f); // down right back
		glm::vec3 DLB = glm::vec3(0.0f); // down left back
	};

	static rubiksCubePoints fetchFurthestPoints(const std::vector<glm::vec3>& points);
	static rubiksCubePoints fetchFurthestVertices(const std::vector<Vertex>& vertices);

	static AABB createAABBfromRubiksCubePoints(const rubiksCubePoints& points);
	static AABB createAABBfromVertices(const std::vector<Vertex>& vertices);
	static AABB createAABBfromPoints(const std::vector<glm::vec3>& points);

	static rubiksCubePoints transformRubiks(const rubiksCubePoints& points, glm::mat4 matrix);

	// AABB collision detection
	// AABB vs AABB
	static HitResult AABBvsAABB(
		const glm::vec3& posA, const glm::vec3& sizeA,
		const glm::vec3& posB, const glm::vec3& SizeB);

	// AABB vs Point
	static HitResult AABBvsPoint(
		const glm::vec3& posA, const glm::vec3& sizeA,
		const glm::vec3& point);

	// AABB vs Ray
	static HitResult AABBvsRay(const glm::vec3& posA, const glm::vec3& sizeA,
		glm::vec3 rayOrigin, glm::vec3 rayDir);
	
	static HitResult AABBvsSphere(const glm::vec3 posB, const glm::vec3 scaleB, const glm::vec3 pointS, const float radius);


	// triangle

	static bool TrianglevsPoint(
		const glm::vec3& P, const glm::vec3& A, const glm::vec3& B, const glm::vec3& C, float epsilon);

	static bool RayVsTriangleSimple(glm::vec3 rayOrigin, glm::vec3 rayDir, glm::vec3 A, glm::vec3 B, glm::vec3 C);

	static HitResult RayVsTriangle(glm::vec3 rayOrigin, glm::vec3 rayDir, glm::vec3 A, glm::vec3 B, glm::vec3 C);

	static HitResult TrianglevsTriangle(
		const glm::vec3& v0A, const glm::vec3& v1A, const glm::vec3& v2A,
		const glm::vec3& v0B, const glm::vec3& v1B, const glm::vec3& v2B);

private:

	static void projectVertex(
		const glm::vec3& A, const glm::vec3& B, const glm::vec3& C,
		const glm::vec3& normal,
		float& min, float& max
	);

	static bool doesVertexOverlap(const glm::vec3& v0A, const glm::vec3& v1A, const glm::vec3& v2A,
		const glm::vec3& v0B, const glm::vec3& v1B, const glm::vec3& v2B, const glm::vec3& normal);

public:

	// sphere

	// sphere vs sphere
	static HitResult SpherevsSphere(
		const glm::vec3& P1, const glm::vec3& P2,
		float R1, float R2); // two points and radius

	static bool SphereVsPoint(glm::vec3 spherePoint, float radius, glm::vec3 point);

	//
	static bool isBoxInFrustum(const glm::vec3& worldPos, const glm::vec3& Scale, const glm::mat4& view, const glm::mat4& projection);



private:

};

#endif