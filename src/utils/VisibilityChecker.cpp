#include "VisibilityChecker.h"

bool VisibilityChecker::isInRange(glm::vec3 victimPosition, glm::vec3 viewPosition, float range) {
	float distance = glm::distance(victimPosition, viewPosition);

	return distance <= range;
}