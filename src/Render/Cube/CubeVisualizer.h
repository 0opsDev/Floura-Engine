#ifndef CUBEVISUALIZER_CLASS_H
#define CUBEVISUALIZER_CLASS_H

#include<iostream>
#include <glad/gl.h>
#include<string>
#include<array>
#include <stb/stb_image.h>
#include "render/Shader/shaderClass.h"
#include <camera/Camera.h>

class CubeVisualizer
{
public:
	unsigned int cubeVAO, cubeVBO, cubeEBO;
	std::string DefaultSkyboxPath;

	CubeVisualizer();

	~CubeVisualizer();

	void skyboxBuffer();

	void draw(glm::vec3 position,
		glm::vec3 scale, glm::vec3 colour, bool hasWireframe);
};

#endif // CUBEVISUALIZER_CLASS_H
