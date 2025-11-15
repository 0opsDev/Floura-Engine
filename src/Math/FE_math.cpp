#include "FE_math.h"
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <algorithm>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

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

glm::mat4 FE_Math::composeMatrix(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation)
{
    glm::mat4 mTransform = glm::mat4(1.0f);
    glm::mat4 mRotation = glm::mat4(1.0f);
    glm::mat4 mScale = glm::mat4(1.0f);

    mTransform = glm::translate(mTransform, position);
    
    /*
        mRotation = glm::rotate(mRotation, glm::radians(rotation.x), glm::vec3(1, 0, 0));
    mRotation = glm::rotate(mRotation, glm::radians(rotation.y), glm::vec3(0, 1, 0));
    mRotation = glm::rotate(mRotation, glm::radians(rotation.z), glm::vec3(0, 0, 1));
    
    */


    glm::quat q = glm::quat(glm::radians(rotation));
    mRotation = glm::toMat4(q);

    mScale = glm::scale(mScale, scale);

    return (mTransform * mRotation * mScale);
}

float FE_Math::calculateCenter1D(glm::vec2 input)
{
    return (input.x + input.y) / 2.0f;
}

float FE_Math::distanceFromTwoPoints1D(glm::vec2 input)
{
    return glm::abs(input.x - input.y);
}

float FE_Math::furthestPoint(glm::vec2 input)
{
    return glm::max(input.x, input.y);
}