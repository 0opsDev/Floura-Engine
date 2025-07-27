#include "FE_math.h"
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <algorithm>
#include <cmath>


bool FE_Math::isInRange(glm::vec3 victimPosition, glm::vec3 viewPosition, float range) {
	float distance = glm::distance(victimPosition, viewPosition);

	return distance <= range;
}

// goes towards min and max depending on distance from range
float FE_Math::GetProximityValueLinear(const glm::vec3& object_position, const glm::vec3& camera_position, float radius, float min, float max) {
    if (radius <= 0.0f) {
        return max;
    }

    float current_distance = glm::distance(object_position, camera_position);

    current_distance = std::clamp(current_distance, 0.0f, radius);

    float normalized_t = current_distance / radius;

    float linear_interpolator = 1.0f - normalized_t;

    float result = min + linear_interpolator * (max - min);

    return std::clamp(result, min, max);
}