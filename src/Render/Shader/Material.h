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

	uint64_t modelShaderUUID;
	uint64_t modelGpassShaderUUID;
	uint64_t modelMpassShaderUUID;
	bool hasMaskPass = false;
	//Shader ModelShader;
	//Shader ModelGpassShader;

	std::string materialPath;

	void LoadMaterial(std::string path);

	void ClearMaterial();

	void update();

	void updateForwardLights();

private:
	std::string type;
	
	// regular
	std::string FragmentShaderPath;
	std::string VertexShaderPath;
	std::string GeometryShaderPath;
	//GPS
	std::string FragmentGPShaderPath;
	std::string VertexGPShaderPath;
	std::string GeometryGPShaderPath;
	//MPS MASK PASS
	std::string FragmentMPShaderPath;
	std::string VertexMPShaderPath;
	std::string GeometryMPShaderPath;

	void jsonLoad(std::string path);
};

#endif // MATERIAL_CLASS_H
