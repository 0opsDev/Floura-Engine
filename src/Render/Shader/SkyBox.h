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
	static unsigned int skyboxVAO, skyboxVBO, skyboxEBO;
	static std::string DefaultSkyboxPath;

	static void init(std::string PathName);

	static void LoadSkyBoxTexture(std::string PathName);

	static void skyboxBuffer();

	static void draw(unsigned int width, unsigned int height);

	static void Delete();
};

#endif // SKYBOX_CLASS_H
