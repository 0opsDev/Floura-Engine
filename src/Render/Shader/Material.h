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

	void updateForwardLights();

private:
	std::string type;
	std::string FragmentShaderPath;
	std::string VertexShaderPath;
	std::string FragmentGPShaderPath;
	std::string VertexGPShaderPath;
	std::string GeometryShaderPath;
	std::string GeometryGPShaderPath;

	void updateTime();

	void jsonLoad(std::string path);
};

#endif // MATERIAL_CLASS_H
