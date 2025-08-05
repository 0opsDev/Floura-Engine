#ifndef CUBEMAP_CLASS_H
#define CUBEMAP_CLASS_H

#include<iostream>
#include<glad/glad.h>
#include<string>
#include<array>
#include <stb/stb_image.h>
#include <utils/init.h>
#include<fstream>
#include<sstream>
#include<cerrno>
#include <json/json.hpp>

using json = nlohmann::json;

class Cubemap
{
public:

	unsigned int ID;

	void LoadCubeMapTexture(std::string PathName);

};

#endif
