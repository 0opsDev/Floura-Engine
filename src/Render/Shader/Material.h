#ifndef MATERIAL_CLASS_H
#define MATERIAL_CLASS_H

#include<iostream>
#include<string>
#include<array>
#include "shaderClass.h"
#include <json/json.hpp>
#include "Render/Handler/ShaderHandler.h"
#include "xhash"

using json = nlohmann::json;

class Material
{
public:

	int test = 0;

	uint64_t modelShaderUUID;
	uint64_t modelGpassShaderUUID;

	//Shader ModelShader;
	//Shader ModelGpassShader;

	std::string materialPath;

	void LoadMaterial(std::string path);

	void ClearMaterial();

	void update();

	void updateForwardLights();

private:
	std::string type;
	std::string FragmentShaderPath;
	std::string VertexShaderPath;
	std::string GeometryShaderPath;
	std::string FragmentGPShaderPath;
	std::string VertexGPShaderPath;
	std::string GeometryGPShaderPath;

	void jsonLoad(std::string path);
};

#endif // MATERIAL_CLASS_H
