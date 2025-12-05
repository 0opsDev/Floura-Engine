#ifndef SKYBOX_CLASS_H
#define SKYBOX_CLASS_H

#include<iostream>
#include<glad/glad.h>
#include<string>
#include<array>
#include <stb/stb_image.h>
#include "Render/Shader/shaderClass.h"
#include <camera/Camera.h>
#include "Render/Object/Cubemap.h"

class Skybox
{
public:
	static std::string DefaultSkyboxPath;
	static bool DoSbRGBA;

	static void init();

	static void LoadSkyBoxTexture(std::string PathName);

	static void draw();

	static Cubemap* SkyboxCubemap;

	static void Delete();

	static glm::vec3 rotation;


private:
	static unsigned int VAO, VBO, EBO;
};

#endif // SKYBOX_CLASS_H
