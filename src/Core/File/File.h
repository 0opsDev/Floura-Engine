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
	static std::pair<std::string, std::string> getShaderPaths(int vertIndex, int fragIndex);

	static std::vector<std::tuple<Model, int, glm::vec3, glm::quat, glm::vec3>> loadModelsFromJson(const std::string& jsonFilePath);

	static void loadShaderProgram(int VertNum, int FragNum, Shader& shaderProgram); //shader program switcher

};

#endif // FILE_H