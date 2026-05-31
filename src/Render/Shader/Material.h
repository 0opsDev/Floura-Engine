#ifndef MATERIAL_CLASS_H
#define MATERIAL_CLASS_H

#include<iostream>
#include<string>
#include<array>
#include "shaderClass.h"
#include <json/json.hpp>
#include "Render/Handler/ShaderHandler.h"
#include "xhash"
#include "Render/Handler/UniformManager.h"


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
	
	void compileUniforms();
	
	uint64_t createUniform(UniformManager::uniformTypeEnum type, std::string name);
	void createUniformW_ID(UniformManager::uniformTypeEnum type, std::string name, uint64_t UUID);
	
	struct uniformPair
	{
		uint64_t handle; // ID for search
		int uniformIndexes; // uniform for quick lookup
	};
	
	std::vector <uniformPair> uniformsPairs;

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
