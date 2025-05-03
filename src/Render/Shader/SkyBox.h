#ifndef SKYBOX_CLASS_H
#define SKYBOX_CLASS_H

#include<iostream>
#include<glad/glad.h>
#include<string>
#include<array>
#include <stb/stb_image.h>
#include <utils/init.h>
#include "shaderClass.h"
#include <camera/Camera.h>

class Skybox
{
public:
	unsigned int cubemapTexture;
	unsigned int skyboxVAO, skyboxVBO, skyboxEBO;
	static float s_skyboxVertices[24]; // Adjust the size as needed
	static unsigned int s_skyboxIndices[36]; // Adjust the size as needed
	Skybox();

	void skyboxBuffer();

	void skyboxdraw(Shader skyboxShader, Camera camera, GLfloat skyRGBA[], unsigned int width, unsigned int height);

};

#endif // SKYBOX_CLASS_H
