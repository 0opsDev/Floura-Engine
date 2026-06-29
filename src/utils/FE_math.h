#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#include <vector>
#ifndef FE_MATH_CLASS_H
#define FE_MATH_CLASS_H

class FE_Math
{
public:

	static bool isInRange(glm::vec3 victimPosition, glm::vec3 viewPosition, float range);

	static float GetProximityValueLinear(const glm::vec3& object_position, const glm::vec3& camera_position, float radius, float min, float max);

	static glm::mat4 composeMatrix(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation);

	static float calculateCenter1D(glm::vec2 input);

	static float distanceFromTwoPoints1D(glm::vec2 input);

	static float furthestPoint(glm::vec2 input);

	static float magnitude(glm::vec3 v);

	static float AreaOfTriangle(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3);

	static glm::vec3 RadiansToNormal(float yawRad, float pitchRad);

	static void NormalToRadians(glm::vec3 normal, float& yawRad, float& pitchRad);

	static glm::vec3 getForwardFromMatrix(const glm::mat4& matrix);

	static glm::vec3 getForwardFromViewMatrix(const glm::mat4& viewMatrix);

	static glm::vec2 findTwoFurthestPointsX(std::vector<glm::vec3> points);

	static glm::vec2 findTwoFurthestPointsY(std::vector<glm::vec3> points);

	static glm::vec2 findTwoFurthestPointsZ(std::vector<glm::vec3> points);

	static void transformPoint(glm::vec3& point, glm::mat4 matrix);

	// pad vec3 from 0.0f
	static glm::vec3 pad(glm::vec3 value, float padding);

	static glm::vec3 normalFrom2Points(glm::vec3 p1, glm::vec3 p2);

	static glm::mat4 composeMatrixWDegrees(glm::vec3 p, glm::vec3 s, glm::vec3 r);

	static glm::mat4 composeMatrixwQuaternion(glm::vec3 p, glm::vec3 s, glm::quat r);

	static int floatToByteRGB(float input);

	static float ByteToFloatRGB(int input);
	
	static glm::mat4 createHaltonJitterProjectionMatrix(glm::mat4 matrix, glm::vec2 jitter, int height, int width);
	
	static int calculateLODLevel(glm::vec3 vPosition, glm::vec3 cameraPosition, float transitionDistance, int maxLOD);
};

#endif // FE_MATH_CLASS_H