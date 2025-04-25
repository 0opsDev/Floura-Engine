#ifndef FRAMEBUFFER_CLASS_H
#define FRAMEBUFFER_CLASS_H

#include<iostream>
#include<glad/glad.h>

class Framebuffer
{
public:
	
	void setupMainFBO(unsigned int& viewVAO, unsigned int& viewVBO, unsigned int& FBO, unsigned int& frameBufferTexture, unsigned int& RBO, unsigned int width, unsigned int height, const float* ViewportVerticies);

	void setupSecondFBO(unsigned int& FBO, unsigned int& frameBufferTexture, unsigned int& RBO, unsigned int width, unsigned int height);

};

#endif
