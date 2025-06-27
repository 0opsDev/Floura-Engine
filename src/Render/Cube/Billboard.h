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
	unsigned int iteration;
	float TimeAccumulatorBillboard;
	std::string singleTexturePath;

	std::string TexturePath;
	std::vector<std::string> TextureNames;

	void init(std::string path);
	// this path is json
	void initSeq(std::string path);

	void LoadBillBoardTexture(std::string path);
	void LoadSequence(std::string path);
	void UpdateSequence(int tickrate);

	void skyboxBuffer();

	void draw(bool doPitch, float x, float y, float z, float ScaleX, float ScaleY, float ScaleZ);

	void Delete();
};

#endif // BILLBOARD_CLASS_H
