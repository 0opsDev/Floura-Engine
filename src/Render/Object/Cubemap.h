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

using json = nlohmann::json;

class Cubemap
{
public:

	std::string path;
	GLuint64 handle; /// for bindless textures
	unsigned int ID;

	Cubemap(std::string path);
	~Cubemap();
};

#endif
