#include "Material.h"
#include <Core/Render.h>
#include <Scene/LightingHandler.h>
#include <utils/logConsole.h>
#include "Scene/scene.h"

/*
what should a material do in order

0. clear any previous material variables
1. unload from path (lets not have it in the form of constuctor for ease)
2. init shaders
3. send uniforms to shader
4. ImGuiOverlay

addionally, there should be a material content object for the content window function under the ImGuiWindow class.
no need to save shader there will be a shader editor for that, i think for per model there should be a material savefile tho (it'll be figured out later)
*/



void Material::LoadMaterial(std::string path)
{
	// Clear previous material variables
	ClearMaterial();
	materialPath = path;
	jsonLoad(path);
	if (type == "Standard")
	{
		modelShaderUUID = ShaderHandler::createShader((VertexShaderPath).c_str(), (FragmentShaderPath).c_str());
		modelGpassShaderUUID = ShaderHandler::createShader((VertexGPShaderPath).c_str(), (FragmentGPShaderPath).c_str());

		//ModelShader.LoadShader((VertexShaderPath).c_str(), (FragmentShaderPath).c_str());
		//ModelGpassShader.LoadShader((VertexGPShaderPath).c_str(), (FragmentGPShaderPath).c_str());
	}
	else if (type == "Geometry")
	{
		modelShaderUUID = ShaderHandler::createGeometry((VertexShaderPath).c_str(), (FragmentShaderPath).c_str(), (GeometryShaderPath).c_str());
		modelGpassShaderUUID = ShaderHandler::createGeometry((VertexGPShaderPath).c_str(), (FragmentGPShaderPath).c_str(), (GeometryGPShaderPath).c_str());

		//ModelShader.LoadShaderGeom((VertexShaderPath).c_str(), (FragmentShaderPath).c_str(), (GeometryShaderPath).c_str());
		//ModelGpassShader.LoadShaderGeom((VertexGPShaderPath).c_str(), (FragmentGPShaderPath).c_str(), (GeometryGPShaderPath).c_str());
	}

	// should prolly add back
}

void Material::ClearMaterial()
{
	ShaderHandler::removeInstancewUUID(modelShaderUUID);
	ShaderHandler::removeInstancewUUID(modelGpassShaderUUID);

	//ModelShader.Delete();
	//ModelGpassShader.Delete();
}

void Material::update()
{
	int modelShaderIndex = ShaderHandler::fetchShaderIndex(modelShaderUUID);
	int modelGPShaderIndex = ShaderHandler::fetchShaderIndex(modelGpassShaderUUID);

	//ModelShader.Activate();
	//Scene::maincamera.Matrix(ModelShader, "camMatrix");
	//ModelGpassShader.Activate();
	//Scene::maincamera.Matrix(ModelGpassShader, "camMatrix");
	// this is where we activate and send off all the uniforms to the shader
	ShaderHandler::shaderObjects[modelShaderIndex].Shader.Activate();
	Scene::maincamera.Matrix(ShaderHandler::shaderObjects[modelShaderIndex].Shader, "camMatrix"); // Send Camera Matrix To Shader Prog
	ShaderHandler::shaderObjects[modelGPShaderIndex].Shader.Activate();
	Scene::maincamera.Matrix(ShaderHandler::shaderObjects[modelGPShaderIndex].Shader, "camMatrix");

	//ModelShader.Activate();
	//ModelShader.setFloat("deltatime", TimeUtil::deltatime);
	//ModelShader.setFloat("time", glfwGetTime());

	//ModelGpassShader.Activate();
	//ModelGpassShader.setFloat("deltatime", TimeUtil::deltatime);
	//ModelGpassShader.setFloat("time", glfwGetTime());

	ShaderHandler::shaderObjects[modelShaderIndex].Shader.Activate();
	ShaderHandler::shaderObjects[modelShaderIndex].Shader.setFloat("deltatime", TimeUtil::deltatime);
	ShaderHandler::shaderObjects[modelShaderIndex].Shader.setFloat("time", glfwGetTime());

	ShaderHandler::shaderObjects[modelGPShaderIndex].Shader.Activate();
	ShaderHandler::shaderObjects[modelGPShaderIndex].Shader.setFloat("deltatime", TimeUtil::deltatime);
	ShaderHandler::shaderObjects[modelGPShaderIndex].Shader.setFloat("time", glfwGetTime());
}

void Material::updateForwardLights() {
	int modelShaderIndex = ShaderHandler::fetchShaderIndex(modelShaderUUID);
	LightingHandler::update(ShaderHandler::shaderObjects[modelShaderIndex].Shader);
}


void Material::jsonLoad(std::string path) 
{
	std::ifstream file(path);
	if (!file.is_open()) {
		std::cout << "Material Failed to open file: " << path << std::endl;
		return;
	}
	json modelFileData;
	try {
		file >> modelFileData;
	}
	catch (const nlohmann::json::parse_error& e) {
		// This catch block specifically handles JSON parsing errors,
		// which gives more precise error information from the library.
		std::cout << "JSON Parse Error loading model data: " << e.what() << std::endl;
		std::cout << "Error byte position: " << e.byte << std::endl; // Specific to nlohmann::json
	}
	catch (const std::ios_base::failure& e) {
		// This catch block handles file I/O errors (e.g., file not found, permission issues).
		std::cout << "File I/O Error loading model data: " << e.what() << std::endl;
	}
	catch (const std::exception& e) {
		// A general catch-all for any other std::exception derived errors.
		std::cout << "An unexpected error occurred loading model data: " << e.what() << std::endl;
	}
	file.close();

	for (const auto& item : modelFileData) {

		type = item.at("type").get<std::string>();

		FragmentShaderPath = item.at("FragShader").get<std::string>();
		VertexShaderPath = item.at("VertexShader").get<std::string>();
		FragmentGPShaderPath = item.at("GPassFragShader").get<std::string>();
		VertexGPShaderPath = item.at("GPassVertShader").get<std::string>();
		// if they exsist 
		if (item.contains("GeomShader")) {
			GeometryShaderPath = item.at("GeomShader").get<std::string>();
		}
		if (item.contains("GeometryGPShaderPath")) {
			GeometryGPShaderPath = item.at("GeometryGPShaderPath").get<std::string>();
		}

	}
	std::cout << "Loaded Scene Models from: " << path << std::endl;
}