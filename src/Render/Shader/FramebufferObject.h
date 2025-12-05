#ifndef FRAMEBUFFER_OBJ_CLASS_H
#define FRAMEBUFFER_OBJ_CLASS_H
#include"string.h"
#include <glm/fwd.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

class FramebufferObject
{
public:
	bool doRBO = true;
	unsigned int FBO, RBO, texture;
	// x - width, h - height
	glm::vec2 resolution;
	void create(glm::vec2 resolution, bool doRBO);
	void Delete();

	void resizeResolution(glm::vec2 resolution);

private:
	void genBuffers();
};

#endif
