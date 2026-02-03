#ifndef CUBEMAP_CLASS_H
#define CUBEMAP_CLASS_H

#include<iostream>
#include <glad/gl.h>
#include<string>
#include<array>
#include <stb/stb_image.h>
#include<fstream>
#include<sstream>
#include<cerrno>
#include <json/json.hpp>
#include <Render/Shader/shaderClass.h>
//#include <Render/Object/ModelAssimp.h>


class Model;

using json = nlohmann::json;

class Cubemap
{
public:

	glm::vec2 res;
	std::string path;
	GLuint64 handle; /// for bindless textures
	unsigned int ID;

	Cubemap();
	void loadCubeMap(std::string path);

	void bind(int unit);
	void unbind();

	void cubemapToShader(Shader& shader, int unit);
	void cubemapToUUIDShader(const char* uniform, Shader& shader);

	void drawCubeMap(Shader shader, std::vector<Model*>& Models, glm::vec2 resolution);
	void resizeCubeMap(glm::vec2 resolution);
	~Cubemap();
};

#endif
