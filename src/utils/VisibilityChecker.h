#ifndef VIS_CHECKER_UTIL_H
#define VIS_CHECKER_UTIL_H
#include <glm/fwd.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/gtx/norm.hpp>

class VisibilityChecker {
public:

	static bool isInRange(glm::vec3 victimPosition, glm::vec3 viewPosition, float range);


};

#endif // VIS_CHECKER_UTIL_H
