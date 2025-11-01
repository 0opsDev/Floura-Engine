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
#include "Render/Object/Texture.h"
#include "Core/Render.h"

class BillBoard
{
public:
	unsigned int cubeVAO, cubeVBO, cubeEBO;
	std::string DefaultSkyboxPath; 
	unsigned int iteration;
	float TimeAccumulatorBillboard;
	std::string singleTexturePath;
	RenderClass::transformation globalTransformation;

	void init(std::string path);
	// this path is json
	void initSeq(std::string path);
	void LoadSequence(std::string path);
	void UpdateSequence(int tickrate);

	void buffer();
	
	void setDoPitch(bool doPitch);

	void updatePosition(glm::vec3 Position);

	void updateScale(glm::vec3 Scale);

	void draw();

	void Delete();

private:
	bool doPitch = true;
	Texture Tex;
};

#endif // BILLBOARD_CLASS_H
