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
	static unsigned int cubemapTexture;
	static unsigned int skyboxVAO, skyboxVBO, skyboxEBO;
	static float s_skyboxVertices[24]; // Adjust the size as needed
	static unsigned int s_skyboxIndices[36]; // Adjust the size as needed
	static std::string DefaultSkyboxPath;

	static void init(std::string PathName);

	static void LoadSkyBoxTexture(std::string PathName);

	static void skyboxBuffer();

	static void draw(Camera camera, GLfloat skyRGBA[], unsigned int width, unsigned int height);

	static void Delete();
};

#endif // SKYBOX_CLASS_H
