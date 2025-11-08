#ifndef CUBEVISUALIZER_CLASS_H
#define CUBEVISUALIZER_CLASS_H

#include<iostream>
#include<glad/glad.h>
#include<string>
#include<array>
#include <stb/stb_image.h>
#include <utils/init.h>
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

	void draw(float x, float y, float z, float ScaleX, float ScaleY, float ScaleZ, glm::vec3 colour);
};

#endif // CUBEVISUALIZER_CLASS_H
