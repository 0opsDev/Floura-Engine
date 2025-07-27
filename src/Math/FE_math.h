#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#ifndef FE_MATH_CLASS_H
#define FE_MATH_CLASS_H

class FE_Math
{
public:

	static bool isInRange(glm::vec3 victimPosition, glm::vec3 viewPosition, float range);

	static float GetProximityValueLinear(const glm::vec3& object_position, const glm::vec3& camera_position, float radius, float min, float max);

};

#endif // FE_MATH_CLASS_H