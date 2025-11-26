#ifndef FRAMEBUFFER_OBJ_CLASS_H
#define FRAMEBUFFER_OBJ_CLASS_H
#include<render/Shader/shaderClass.h>
#include"string.h"

class FramebufferObject
{
public:
	bool doRBO = true;
	unsigned int FBO, RBO, texture;
	// x - width, h - height
	glm::vec2 resolution;
	FramebufferObject(glm::vec2 resolution, bool doRBO);
	~FramebufferObject();

	void resizeResolution(glm::vec2 resolution);

private:
	void genBuffers();
};

#endif
