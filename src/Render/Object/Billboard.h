#ifndef BILLBOARD_CLASS_H
#define BILLBOARD_CLASS_H

#include<iostream>
#include <glad/gl.h>
#include<string>
#include<array>
#include <stb/stb_image.h>
#include "render/Shader/shaderClass.h"
#include <camera/Camera.h>
#include "Render/Object/Texture.h"
#include <Render/Handler/RenderClass.h>

class BillBoard
{
public:
	//std::string DefaultSkyboxPath;
	RenderClass::transformation globalTransformation;

	BillBoard(std::string path);
	~BillBoard();
	// this path is json

	void updatePosition(glm::vec3 Position);

	void updateScale(glm::vec3 Scale);

	void drawShadowMap();
	void draw();
	bool doPitch = true;
	Texture Tex;

private:

	unsigned int cubeVAO, cubeVBO, cubeEBO;

	void drawF(glm::mat4 modelMatrix, Shader shader, glm::mat4 camMatrix);


	void buffer();
};

#endif // BILLBOARD_CLASS_H
