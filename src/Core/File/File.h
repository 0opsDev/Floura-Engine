#ifndef FILE_H
#define FILE_H
#include <json/json.hpp>
#include <utility>
#include <string>
#include <stdexcept>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>
#include <glm/fwd.hpp>
#include <Render/Model/Model.h>
#include <utils/init.h>
#include <glm/gtx/string_cast.hpp>


using json = nlohmann::json;

class FileClass
{
public:

	// Text editor
	static std::string currentPath; // Current working directory path
	static std::string Contents;

	static void saveContents();
	static void loadContents();

	// General
	static std::pair<std::string, std::string> getShaderPaths(int vertIndex, int fragIndex);

	static std::vector<std::tuple<Model, int, glm::vec3, glm::vec4, glm::vec3, int>> loadModelsFromJson(const std::string& jsonFilePath);

	static void loadShaderProgram(int VertNum, int FragNum, Shader& shaderProgram); //shader program switcher

};

#endif // FILE_H