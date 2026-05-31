#include "Collision.h"
#include <glm/common.hpp>
#include <algorithm>
#include <glm/geometric.hpp>
#include <utils/FE_math.h>
#include <array>
#include <glm/gtx/norm.hpp>

Collision::Frustum Collision::createFrustumFromCamera(glm::mat4 m)
{
	
	Frustum f;
	
	f.leftFace.normal.x = m[0][3] + m[0][0];
	f.leftFace.normal.y = m[1][3] + m[1][0];
	f.leftFace.normal.z = m[2][3] + m[2][0];
	f.leftFace.distance = m[3][3] + m[3][0];

	f.rightFace.normal.x = m[0][3] - m[0][0];
	f.rightFace.normal.y = m[1][3] - m[1][0];
	f.rightFace.normal.z = m[2][3] - m[2][0];
	f.rightFace.distance = m[3][3] - m[3][0];

	f.bottomFace.normal.x = m[0][3] + m[0][1];
	f.bottomFace.normal.y = m[1][3] + m[1][1];
	f.bottomFace.normal.z = m[2][3] + m[2][1];
	f.bottomFace.distance = m[3][3] + m[3][1];

	f.topFace.normal.x = m[0][3] - m[0][1];
	f.topFace.normal.y = m[1][3] - m[1][1];
	f.topFace.normal.z = m[2][3] - m[2][1];
	f.topFace.distance = m[3][3] - m[3][1];

	f.nearFace.normal.x = m[0][3] + m[0][2];
	f.nearFace.normal.y = m[1][3] + m[1][2];
	f.nearFace.normal.z = m[2][3] + m[2][2];
	f.nearFace.distance = m[3][3] + m[3][2];

	f.farFace.normal.x = m[0][3] - m[0][2];
	f.farFace.normal.y = m[1][3] - m[1][2];
	f.farFace.normal.z = m[2][3] - m[2][2];
	f.farFace.distance = m[3][3] - m[3][2];
	
	auto normalizePlane = [](plane& p) {
		float mag = glm::length(p.normal);
		p.normal /= mag;
		p.distance /= mag;
	};
	
	normalizePlane(f.leftFace);
	normalizePlane(f.rightFace);
	normalizePlane(f.bottomFace);
	normalizePlane(f.topFace);
	normalizePlane(f.nearFace);
	normalizePlane(f.farFace);

	return f;
}

bool Collision::showBoxCollider = false;

Collision::rubiksCubePoints Collision::fetchFurthestPoints(const std::vector<glm::vec3>& points)
{
	// 8 furthest points
	rubiksCubePoints fp;
	// early skip if points is empty
	if (points.empty()) 
	{
		return fp;
	}

	float minX, maxX, minY, maxY, minZ, maxZ;
	minX = maxX = points[0].x;
	minY = maxY = points[0].y;
	minZ = maxZ = points[0].z;

	// find the min and max points
	for (const auto& p : points) {
		if (p.x < minX) minX = p.x;
		if (p.x > maxX) maxX = p.x;
		if (p.y < minY) minY = p.y;
		if (p.y > maxY) maxY = p.y;
		if (p.z < minZ) minZ = p.z;
		if (p.z > maxZ) maxZ = p.z;
	}

	// up
	fp.URF = glm::vec3(maxX, maxY, maxZ);
	fp.ULF = glm::vec3(minX, maxY, maxZ);
	fp.URB = glm::vec3(maxX, maxY, minZ);
	fp.ULB = glm::vec3(minX, maxY, minZ);
	// down
	fp.DRF = glm::vec3(maxX, minY, maxZ);
	fp.DLF = glm::vec3(minX, minY, maxZ);
	fp.DRB = glm::vec3(maxX, minY, minZ);
	fp.DLB = glm::vec3(minX, minY, minZ);

	// return furthest points
	return fp;
}

Collision::rubiksCubePoints Collision::fetchFurthestVertices(const std::vector<Vertex>& vertices)
{
	// 8 furthest points
	rubiksCubePoints fp;
	// early skip if points is empty
	if (vertices.empty())
	{
		return fp;
	}

	float minX, maxX, minY, maxY, minZ, maxZ;
	minX = maxX = vertices[0].position.x;
	minY = maxY = vertices[0].position.y;
	minZ = maxZ = vertices[0].position.z;

	// find the min and max points
	for (const auto& p : vertices) {
		if (p.position.x < minX) minX = p.position.x;
		if (p.position.x > maxX) maxX = p.position.x;
		if (p.position.y < minY) minY = p.position.y;
		if (p.position.y > maxY) maxY = p.position.y;
		if (p.position.z < minZ) minZ = p.position.z;
		if (p.position.z > maxZ) maxZ = p.position.z;
	}

	// up
	fp.URF = glm::vec3(maxX, maxY, maxZ);
	fp.ULF = glm::vec3(minX, maxY, maxZ);
	fp.URB = glm::vec3(maxX, maxY, minZ);
	fp.ULB = glm::vec3(minX, maxY, minZ);
	// down
	fp.DRF = glm::vec3(maxX, minY, maxZ);
	fp.DLF = glm::vec3(minX, minY, maxZ);
	fp.DRB = glm::vec3(maxX, minY, minZ);
	fp.DLB = glm::vec3(minX, minY, minZ);

	// return furthest points
	return fp;
}

Collision::rubiksCubePoints Collision::aabbToRubixCubePoints(const glm::vec3 p, glm::vec3 s)
{
	rubiksCubePoints nPoints;

	
	//glm::vec3 hs = s * 0.5f; // half scale
	glm::vec3 hs = s; // half scale

	glm::vec3 pos = glm::vec3(p.x + hs.x, p.y + hs.y, p.z + hs.z);
	glm::vec3 neg = glm::vec3(p.x - hs.x, p.y - hs.y, p.z - hs.z);

	//float px = p.x + hs.x; // right
	//float py = p.y + hs.y; //up
	//float pz = p.z + hs.z; // front

	//float nx = p.x - hs.x; // left
	//float ny = p.y - hs.y; // down
	//float nz = p.z - hs.z; // back

	// up
	nPoints.ULF = glm::vec3(neg.x, pos.y, pos.z); // up left front
	nPoints.URF = glm::vec3(pos.x, pos.y, pos.z); // up right front
	nPoints.URB = glm::vec3(pos.x, pos.y, neg.z); // up right back
	nPoints.ULB = glm::vec3(neg.x, pos.y, neg.z); // up left back
	// down
	nPoints.DLF = glm::vec3(neg.x, neg.y, pos.z); // down left front
	nPoints.DRF = glm::vec3(pos.x, neg.y, pos.z); // down right front
	nPoints.DRB = glm::vec3(pos.x, neg.y, neg.z); // down right back
	nPoints.DLB = glm::vec3(neg.x, neg.y, neg.z); // down left back
	return nPoints;
}


static std::array<glm::vec3, 8> getPointsArrayFromRubiksCubePoints(const Collision::rubiksCubePoints& points)
{
	return {
		points.ULF, points.URF, points.URB, points.ULB,
		points.DLF, points.DRF, points.DRB, points.DLB
	};
}

Collision::AABB Collision::createAABBfromRubiksCubePoints(const rubiksCubePoints& points)
{
	// optimise later btw
	Collision::AABB AABB;

	std::array<glm::vec3, 8> pointsArray = getPointsArrayFromRubiksCubePoints(points);

	glm::vec3 min = glm::vec3(std::numeric_limits<float>::max());
	glm::vec3 max = glm::vec3(std::numeric_limits<float>::lowest());

	for (size_t i = 0; i < pointsArray.size(); i++)
	{
		min = glm::min(min, pointsArray[i]);
		max = glm::max(max, pointsArray[i]);
	}
	AABB.position = (min + max) * 0.5f;
	AABB.size = (max - min) * 0.5f;

	return AABB;
}

Collision::AABB Collision::createAABBfromVertices(const std::vector<Vertex>& vertices)
{
	// optimise later btw
	Collision::AABB AABB;

	glm::vec3 min = glm::vec3(std::numeric_limits<float>::max());
	glm::vec3 max = glm::vec3(std::numeric_limits<float>::lowest());

	for (size_t i = 0; i < vertices.size(); i++)
	{
		min = glm::min(min, vertices[i].position);
		max = glm::max(max, vertices[i].position);
	}
	AABB.position = (min + max) * 0.5f;
	AABB.size = (max - min) * 0.5f;

	return AABB;
}

Collision::AABB Collision::createAABBfromPoints(const std::vector<glm::vec3>& points)
{
	// optimise later btw
	Collision::AABB AABB;
	if (points.empty()) return Collision::AABB();

	glm::vec3 min = glm::vec3(std::numeric_limits<float>::max());
	glm::vec3 max = glm::vec3(std::numeric_limits<float>::lowest());

	for (size_t i = 0; i < points.size(); i++)
	{
		min = glm::min(min, points[i]);
		max = glm::max(max, points[i]);
	}
	AABB.position = (min + max) * 0.5f;
	AABB.size = (max - min) * 0.5f;

	return AABB;
}

Collision::rubiksCubePoints Collision::transformRubiks(const rubiksCubePoints& points, glm::mat4 matrix)
{
	Collision::rubiksCubePoints newpoints = points;

	// up
	FE_Math::transformPoint(newpoints.ULF, matrix);
	FE_Math::transformPoint(newpoints.URF, matrix);
	FE_Math::transformPoint(newpoints.URB, matrix);
	FE_Math::transformPoint(newpoints.ULB, matrix);
	// down
	FE_Math::transformPoint(newpoints.DLF, matrix);
	FE_Math::transformPoint(newpoints.DRF, matrix);
	FE_Math::transformPoint(newpoints.DRB, matrix);
	FE_Math::transformPoint(newpoints.DLB, matrix);

	return newpoints;
}

Collision::minmax Collision::returnMinMax(glm::vec3 p, glm::vec3 s)
{
	minmax newMinMax;
	newMinMax.min = p - s,
	newMinMax.max = p + s;
	return newMinMax;
}

glm::vec3 Collision::constrainPoint(glm::vec3 vp, glm::vec3 c2, float cRadius)
{
	// check if inside the radius, otherwise fail
	if  (SphereVsPoint(c2, cRadius, vp)) return vp;
	
	// get normal of two positons from point pointing to victim
	glm::vec3 direction = c2 - vp;
	glm::vec3 normal = glm::normalize(direction);
	// clamp inside
	glm::vec3 clampedPosition = c2 - normal * cRadius;
	
	return clampedPosition;
}

Collision::HitResult Collision::advancedConstrainPoint(glm::vec3 vp, glm::vec3 c2, float cRadius)
{
	HitResult hitResult;
	hitResult.isColliding = false;
	
	// get distance
	float distance = glm::distance(c2, vp);
	// check if inside the radius, otherwise fail (sphere v point collision)
	if  (distance < cRadius){ hitResult.lastHit = vp; return hitResult;}
	
	// is colliding
	hitResult.isColliding = true;
	
	// get normal of two positons from point pointing to victim
	glm::vec3 direction = c2 - vp;
	glm::vec3 normal = glm::normalize(direction);
	hitResult.lastHit = c2 - normal * cRadius; // clamp inside // position of vic
	
	// inverse normal for collisionnormal
	glm::vec3 inverseDirection = vp - c2;
	glm::vec3 inverseNormal = glm::normalize(inverseDirection);
	hitResult.collisionNormal =inverseNormal;
	
	// how deep past the clamp
	// get distance from piviot to vic, use cRadius (range) to subtract so the access is left
	hitResult.depth =distance - cRadius;
	hitResult.distance = distance;

	return hitResult;
}

Collision::HitResult Collision::AABBvsAABB(
	const glm::vec3& posA, const glm::vec3& sizeA,
	const glm::vec3& posB, const glm::vec3& sizeB)
{
	HitResult data;

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

Collision::HitResult Collision::AABBvsPoint(
	const glm::vec3& posA, const glm::vec3& sizeA,
	const glm::vec3& point)
{
	HitResult data;

	glm::vec3 AMin = posA - sizeA;
	glm::vec3 AMax = posA + sizeA;

	data.isColliding = false;
	data.lastHit = glm::vec3(0.0f);
	data.collisionNormal = glm::vec3(0.0f);
	data.distance = 0.0f;

	// Check if the point is inside the AABB
	if (point.x >= AMin.x && point.x <= AMax.x &&
		point.y >= AMin.y && point.y <= AMax.y &&
		point.z >= AMin.z && point.z <= AMax.z)
	{
		data.isColliding = true;
		// Determine the closest face and push the point fully outside
		float leftDist = abs(point.x - AMin.x);
		float rightDist = abs(point.x - AMax.x);
		float bottomDist = abs(point.y - AMin.y);
		float topDist = abs(point.y - AMax.y);
		float frontDist = abs(point.z - AMin.z);
		float backDist = abs(point.z - AMax.z);
		// Find the minimum distance
		float minDist = std::min({ leftDist, rightDist, bottomDist, topDist, frontDist, backDist });
		if (minDist == leftDist) {
			data.collisionNormal = glm::vec3(-1.0f, 0.0f, 0.0f);
			data.lastHit = glm::vec3(AMin.x, point.y, point.z);
		}
		else if (minDist == rightDist) {
			data.collisionNormal = glm::vec3(1.0f, 0.0f, 0.0f);
			data.lastHit = glm::vec3(AMax.x, point.y, point.z);
		}
		else if (minDist == bottomDist) {
			data.collisionNormal = glm::vec3(0.0f, -1.0f, 0.0f);
			data.lastHit = glm::vec3(point.x, AMin.y, point.z);
		}
		else if (minDist == topDist) {
			data.collisionNormal = glm::vec3(0.0f, 1.0f, 0.0f);
			data.lastHit = glm::vec3(point.x, AMax.y, point.z);
		}
		else if (minDist == frontDist) {
			data.collisionNormal = glm::vec3(0.0f, 0.0f, -1.0f);
			data.lastHit = glm::vec3(point.x, point.y, AMin.z);
		}
		else if (minDist == backDist) {
			data.collisionNormal = glm::vec3(0.0f, 0.0f, 1.0f);
			data.lastHit = glm::vec3(point.x, point.y, AMax.z);
		}
	}
	// else no collision

	return data;
}
Collision::HitResult Collision::AABBvsRay(const glm::vec3& posA, const glm::vec3& sizeA,
	glm::vec3 rayOrigin, glm::vec3 rayDir)
{
	HitResult data;
	data.isColliding = false;
	data.collisionNormal = glm::vec3(0.0f);

	glm::vec3 AMin = posA - sizeA;
	glm::vec3 AMax = posA + sizeA;

	// inverted direction
	glm::vec3 invDir = 1.0f / rayDir;

	// calc intersection distances for each axis
	glm::vec3 t0 = (AMin - rayOrigin) * invDir;
	glm::vec3 t1 = (AMax - rayOrigin) * invDir;

	// mae sure tmin is the entry point, tmax is the exit point for each axis
	glm::vec3 tMin = glm::min(t0, t1);
	glm::vec3 tMax = glm::max(t0, t1);

	// find entry and exit tvalues for the ray
	float tNear = std::max({ tMin.x, tMin.y, tMin.z });
	float tFar = std::min({ tMax.x, tMax.y, tMax.z });

	// near > far misses the box
	// far < 0 behind the box

	if (tNear > tFar || tFar < 0) {
		return data;
	}

	data.isColliding = true;
	data.distance = tNear;
	data.lastHit = rayOrigin + (rayDir * tNear);

	if (tNear == tMin.x) {
		data.collisionNormal = glm::vec3(rayDir.x > 0 ? -1.0f : 1.0f, 0.0f, 0.0f);
	}
	else if (tNear == tMin.y) {
		data.collisionNormal = glm::vec3(0.0f, rayDir.y > 0 ? -1.0f : 1.0f, 0.0f);
	}
	else if (tNear == tMin.z) {
		data.collisionNormal = glm::vec3(0.0f, 0.0f, rayDir.z > 0 ? -1.0f : 1.0f);
	}

	return data;
}

Collision::HitResult Collision::AABBvsSphere(const glm::vec3 posB, const glm::vec3 scaleB, const glm::vec3 pointS, const float radius)
{
	HitResult newResult;
	newResult.isColliding = false;

	glm::vec3 AMin = posB - scaleB;
	glm::vec3 AMax = posB + scaleB;

	glm::vec3 p = glm::clamp(pointS, AMin, AMax);

	glm::vec3 v = p - pointS;
	float distanceSquared = glm::dot(v, v);

	if (distanceSquared < (radius * radius))
	{
		newResult.isColliding = true;
		newResult.distance = sqrt(distanceSquared);
		newResult.collisionNormal = (newResult.distance > 0) ? v / newResult.distance : glm::vec3(0, 1, 0);
		newResult.depth = radius - newResult.distance;
	}

	return newResult;
}

bool Collision::TrianglevsPoint(
	const glm::vec3& P, const glm::vec3& A, const glm::vec3& B, const glm::vec3& C, float epsilon)
{
	//Area(PAB) + Area(PBC) + Area(PCA) == Area(ABC))
	float totalarea = FE_Math::AreaOfTriangle(A,B,C);

	// calculate area of sub-triangles
	float areaPAB = FE_Math::AreaOfTriangle(P, A, B);
	float areaPBC = FE_Math::AreaOfTriangle(P, B, C);
	float areaPCA = FE_Math::AreaOfTriangle(P, C, A);

	float areasum = areaPAB + areaPBC + areaPCA;

	// epsilon 0.0001f
	float relativeEpsilon = epsilon * totalarea;
	return std::abs(totalarea - areasum) < relativeEpsilon;
	//return (totalarea == areasum);
}

bool Collision::RayVsTriangleSimple(glm::vec3 rayOrigin, glm::vec3 rayDir, glm::vec3 A, glm::vec3 B, glm::vec3 C)
{
	// normal of triangle
	glm::vec3 normal = glm::normalize(glm::cross(B - A, C - A));

	// check if ray is parallel to triangle
	float dotProduct = glm::dot(normal, rayDir);
	if (std::abs(dotProduct) < 0.0001f) return false;

	// Find the distance 't' to the plane
	float t = glm::dot(A - rayOrigin, normal) / dotProduct;

	// if t is negative, the triangle is behind the ray
	if (t < 0) return false;

	// Find the intersection point
	glm::vec3 P = rayOrigin + (t * rayDir);

	// Check if the intersection point is inside the triangle
	return TrianglevsPoint(P, A, B, C, 0.0001f);
}

Collision::HitResult Collision::RayVsTriangle(glm::vec3 rayOrigin, glm::vec3 rayDir, glm::vec3 A, glm::vec3 B, glm::vec3 C)
{
	Collision::HitResult data;
	data.isColliding = false;
	// normal of triangle
	glm::vec3 normal = glm::normalize(glm::cross(B - A, C - A));
	data.collisionNormal = normal;

	// check if ray is parallel to triangle
	float dotProduct = glm::dot(normal, rayDir);
	if (std::abs(dotProduct) < 0.0001f) return data;

	// Find the distance 't' to the plane
	float t = glm::dot(A - rayOrigin, normal) / dotProduct;

	// if t is negative, the triangle is behind the ray
	if (t < 0) return data;

	// Find the intersection point
	glm::vec3 P = rayOrigin + (t * rayDir);

	// Check if the intersection point is inside the triangle
	if (Collision::TrianglevsPoint(P, A, B, C, 0.0001f))
	{
		data.isColliding = true;
		data.lastHit = P;
		data.distance = t;
	}

	// data.isColliding = false
	return data;
}

Collision::HitResult Collision::SATTrianglevsTriangle(
	const glm::vec3& v0A, const glm::vec3& v1A, const glm::vec3& v2A,
	const glm::vec3& v0B, const glm::vec3& v1B, const glm::vec3& v2B)
{
	HitResult data;
	data.isColliding = false;
	float minOverlap = std::numeric_limits<float>::max();

	// edges
	glm::vec3 edgeA[] = { v1A - v0A, v2A - v1A, v0A - v2A };
	glm::vec3 edgeB[] = { v1B - v0B, v2B - v1B, v0B - v2B };

	// all axis (normnals of faces)
	glm::vec3 testAxis[11];
	testAxis[0] = glm::cross(edgeA[0], edgeA[1]); // normal A
	testAxis[1] = glm::cross(edgeB[0], edgeB[1]); // normal B

	int count = 2;
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			glm::vec3 axis = glm::cross(edgeA[i], edgeB[j]);
			if (glm::length2(axis) > 0.0001f) {
				testAxis[count++] = axis;
			}
		}
	}

	for (int i = 0; i < count; ++i) {
		glm::vec3 axis = glm::normalize(testAxis[i]);
		float minA, maxA, minB, maxB;

		projectVertex(v0A, v1A, v2A, axis, minA, maxA);
		projectVertex(v0B, v1B, v2B, axis, minB, maxB);

		// is there a gap
		if (minA >= maxB || minB >= maxA) {
			return data;
		}

		// overlap
		float overlap = glm::min(maxA, maxB) - glm::max(minA, minB);
		if (overlap < minOverlap) {
			minOverlap = overlap;
			data.collisionNormal = axis;
		}
	}

	// colliding

	data.isColliding = true;
	data.depth = minOverlap;

	// normal
	glm::vec3 centerA = (v0A + v1A + v2A) / 3.0f;
	glm::vec3 centerB = (v0B + v1B + v2B) / 3.0f;
	if (glm::dot(data.collisionNormal, centerB - centerA) < 0) {
		data.collisionNormal = -data.collisionNormal;
	}

	return data;
}

Collision::HitResult Collision::SATTriangleVSAABB(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& AABBpos, const glm::vec3& AABBsize)
{
	HitResult data;
	data.isColliding = false;
	float minOverlap = std::numeric_limits<float>::max();
	
	// edges, normal for triangles
	glm::vec3 vEdges[] = { v1 - v0, v2 - v1, v0 - v2 };
	glm::vec3 vNormal = glm::normalize(glm::cross(vEdges[0], vEdges[1]));
	
	//
	std::vector<glm::vec3> testAxes;
	
	// AABB normals
	testAxes.push_back({1, 0, 0});
	testAxes.push_back({0, 1, 0});
	testAxes.push_back({0, 0, 1});
	
	// push AABB normals into array
	testAxes.push_back(vNormal);
	
	
	glm::vec3 aabbEdges[] = { {1,0,0}, {0,1,0}, {0,0,1} };
	
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			glm::vec3 axis = glm::cross(vEdges[i], aabbEdges[j]);
			if (glm::length2(axis) > 0.0001f)
				testAxes.push_back(glm::normalize(axis));
		}
	}
	
	// rest axes
	for (const auto& axis : testAxes) {
		float minT, maxT, minB, maxB;

		// project points onto axis
		projectVertex(v0, v1, v2, axis, minT, maxT);
		projectAABB(AABBpos, AABBsize, axis, minB, maxB);

		// is there a gap
		if (minT >= maxB || minB >= maxT) {
			return data; // no collision
		}

		// overlap
		float overlap = glm::min(maxT, maxB) - glm::max(minT, minB);
		if (overlap < minOverlap) {
			minOverlap = overlap;
			data.collisionNormal = axis;
		}
	}
	
	// collision
	data.isColliding = true;
	data.depth = minOverlap;
	data.lastHit = AABBpos;

	// normal
	glm::vec3 triCenter = (v0 + v1 + v2) / 3.0f;
	if (glm::dot(data.collisionNormal, triCenter - AABBpos) < 0) {
		data.collisionNormal = -data.collisionNormal;
	}
	
	return data;
}

// SAT

void Collision::projectAABB(const glm::vec3& positon, const glm::vec3& scale, const glm::vec3& normal, float& min, float& max) {
	
	// project centre
	float posProj = glm::dot(positon, normal);
    
	// radius
	float r = scale.x * std::abs(normal.x) + 
			  scale.y * std::abs(normal.y) + 
			  scale.z * std::abs(normal.z);

	// min max
	min = posProj - r;
	max = posProj + r;
}

void Collision::projectVertex(
	const glm::vec3& A, const glm::vec3& B, const glm::vec3& C,
	const glm::vec3& normal,
	float& min, float& max
)
{
	// projected onto line
	float dotA = glm::dot(A, normal);
	float dotB = glm::dot(B, normal);
	float dotC = glm::dot(C, normal);

	// min and max values 
	min = dotA;
	max = dotA;

	if (dotB < min) min = dotB;
	if (dotB > max) max = dotB;
	if (dotC < min) min = dotC;
	if (dotC > max) max = dotC;
}

bool Collision::doesVertexOverlap(const glm::vec3& v0A, const glm::vec3& v1A, const glm::vec3& v2A,
	const glm::vec3& v0B, const glm::vec3& v1B, const glm::vec3& v2B, const glm::vec3& normal)
{
	float minA; float maxA; //A
	float minB; float maxB; //B

	Collision::projectVertex(v0A, v1A, v2A, normal, minA, maxA); // a
	Collision::projectVertex(v0B, v1B, v2B, normal, minB, maxB); // b
	if (minA >= maxB || minB >= maxA)
		return false;
	return true;
}


Collision::HitResult Collision::SpherevsSphere(
	const glm::vec3& P1, const glm::vec3& P2,
	float R1, float R2) // two points and radius
{
	HitResult data;

	// logic 
	// find distance of two points
	float distance = glm::distance(P1, P2);

	// collision is made
	data.isColliding = false;
	if ((R1 + R2) >= distance)
	{
		data.isColliding = true;
		//data.lastHit = 
	}

	return data;
}


bool Collision::SphereVsPoint(glm::vec3 spherePoint, float radius, glm::vec3 point)
{
	// distance between two points
	float distance = glm::distance(spherePoint, point);

	return distance < radius;
}

bool Collision::isBoxInFrustum(const glm::vec3& worldPos, const glm::vec3& Scale, const glm::mat4& view, const glm::mat4& projection)
{
	glm::mat4 pv = projection * view;

	glm::vec4 planes[6];
	planes[0] = pv[3] + pv[0]; // Left
	planes[1] = pv[3] - pv[0]; // Right
	planes[2] = pv[3] + pv[1]; // Bottom
	planes[3] = pv[3] - pv[1]; // Top
	planes[4] = pv[3] + pv[2]; // Near
	planes[5] = pv[3] - pv[2]; // Far
	for (int i = 0; i < 6; i++) {
		float radius = Scale.x * glm::abs(planes[i].x) +
			Scale.y * glm::abs(planes[i].y) +
			Scale.z * glm::abs(planes[i].z);

		float distance = glm::dot(glm::vec3(planes[i]), worldPos) + planes[i].w;
		if (distance < -radius) {
			return false;
		}
	}

	return true;
}

Collision::Sphere Collision::AABBtoSphere(const glm::vec3& posA, const glm::vec3& sizeA)
{
	Sphere nSphere;
	float length = glm::length(sizeA);
	nSphere.radius = length;
	nSphere.position = posA;
	
	return nSphere;
}

bool Collision::AABBtoSphereRangeCull(const glm::vec3& posA, const glm::vec3& sizeA, glm::vec3 point, float distance)
{
	// radius from centre off aabb from longest point
	float sphereRadius = glm::length(sizeA * 1.5f);
	
	// distance between aabb centre and point
	float pDistance = glm::distance(posA, point);
	
	//Sphere edge distance from point
	float edgeDistance = pDistance - sphereRadius;
	
	// check wether edge distance is within distance
	return edgeDistance < distance;
};


