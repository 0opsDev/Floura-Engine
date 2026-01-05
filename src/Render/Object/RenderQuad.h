#ifndef RENDERQUAD_CLASS_H
#define RENDERQUAD_CLASS_H

#include<iostream>
#include<glad/glad.h>
#include<string>
#include<array>
#include <stb/stb_image.h>
#include "render/Shader/shaderClass.h"
#include <camera/Camera.h>

class RenderQuad
{
public:
	unsigned int VAO, VBO, EBO;

	void init();

	void buffer();

	void draw();

	void drawtex(unsigned int& tex);
};

#endif // RENDERQUAD_CLASS_H
