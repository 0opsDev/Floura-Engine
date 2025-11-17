#ifndef FRAMEBUFFER_OBJ_CLASS_H
#define FRAMEBUFFER_OBJ_CLASS_H
#include<render/Shader/shaderClass.h>
#include"string.h"

class FramebufferObject
{
public:

	unsigned int ID;

	// x - width, h - height
	FramebufferObject(glm::vec2 resolution);
	~FramebufferObject();
private:
	Shader shader;
	void genBuffers();
	unsigned int Width, Height;
};

#endif
