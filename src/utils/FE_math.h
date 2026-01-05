#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#include <vector>
#ifndef FE_MATH_CLASS_H
#define FE_MATH_CLASS_H

class FE_Math
{
public:
	static glm::mat4 composeMatrixWDegrees(glm::vec3 p, glm::vec3 s, glm::vec3 r);

};

#endif // FE_MATH_CLASS_H