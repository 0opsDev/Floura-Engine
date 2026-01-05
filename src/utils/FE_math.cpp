#include "FE_math.h"
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <algorithm>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

glm::mat4 FE_Math::composeMatrixWDegrees(glm::vec3 p, glm::vec3 s, glm::vec3 r)
{
    glm::mat4 np = glm::translate(glm::mat4(1.0f), p);
    glm::mat4 nr = glm::mat4(1.0f);
    nr = glm::rotate(nr, glm::radians(r.x), glm::vec3(1, 0, 0));
    nr = glm::rotate(nr, glm::radians(r.y), glm::vec3(0, 1, 0));
    nr = glm::rotate(nr, glm::radians(r.z), glm::vec3(0, 0, 1));
    glm::mat4 ns = glm::scale(glm::mat4(1.0f), s);

    return np * nr * ns;
}
