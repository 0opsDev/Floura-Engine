#ifndef MATERIAL_CLASS_H
#define MATERIAL_CLASS_H

#include<iostream>
#include<string>
#include<array>
#include "shaderClass.h"
#include <json/json.hpp>

using json = nlohmann::json;

class Material
{
public:

	int test = 0;

	Shader ModelShader;
	Shader ModelGpassShader;

	std::string materialPath;

	void LoadMaterial(std::string path);

	void ClearMaterial();

	void update();

	void updateForwardLights(std::vector<glm::vec3>& colour,
		std::vector<glm::vec3>& position, std::vector<glm::vec2>& radiusAndPower,
		std::vector<int>& lightType, std::vector<int>& enabled);

private:
	std::string type;
	std::string FragmentShaderPath;
	std::string VertexShaderPath;
	std::string FragmentGPShaderPath;
	std::string VertexGPShaderPath;

	void updateTime();

	void jsonLoad(std::string path);
};

#endif // MATERIAL_CLASS_H
