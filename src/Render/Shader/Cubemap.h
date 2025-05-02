#ifndef CUBEMAP_CLASS_H
#define CUBEMAP_CLASS_H

#include<iostream>
#include<glad/glad.h>
#include<string>
#include<array>
#include <stb/stb_image.h>
#include <utils/init.h>
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>
#include <json/json.hpp>

using json = nlohmann::json;

class Cubemap
{
public:



	void LoadCubeMapTexture(std::string PathName);

	void cubeboxTexture(unsigned int& cubemapTexture);

};

#endif
