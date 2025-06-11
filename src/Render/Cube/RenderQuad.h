#ifndef RENDERQUAD_CLASS_H
#define RENDERQUAD_CLASS_H

#include<iostream>
#include<glad/glad.h>
#include<string>
#include<array>
#include <stb/stb_image.h>
#include <utils/init.h>
#include "render/Shader/shaderClass.h"
#include <camera/Camera.h>

class RenderQuad
{
public:
	unsigned int cubeVAO, cubeVBO, cubeEBO;

	void init();

	void skyboxBuffer();

	void draw(Shader shader);
};

#endif // RENDERQUAD_CLASS_H
