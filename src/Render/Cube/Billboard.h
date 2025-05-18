#ifndef BILLBOARD_CLASS_H
#define BILLBOARD_CLASS_H

#include<iostream>
#include<glad/glad.h>
#include<string>
#include<array>
#include <stb/stb_image.h>
#include <utils/init.h>
#include "render/Shader/shaderClass.h"
#include <camera/Camera.h>

class BillBoard
{
public:
	unsigned int cubeVAO, cubeVBO, cubeEBO;
	std::string DefaultSkyboxPath;
	unsigned int BBTexture;

	void init(std::string path);

	void LoadBillBoardTexture(std::string path);

	void skyboxBuffer();

	void draw(Camera& camera, bool doPitch, float x, float y, float z, float ScaleX, float ScaleY, float ScaleZ);

	void Delete();
};

#endif // BILLBOARD_CLASS_H
