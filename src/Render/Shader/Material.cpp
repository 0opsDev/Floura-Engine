#include "Material.h"
#include <utils/init.h>
#include <Core/Render.h>
#include <Scene/LightingHandler.h>
#include <utils/logConsole.h>

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
		ModelShader.LoadShader((VertexShaderPath).c_str(), (FragmentShaderPath).c_str());
		ModelGpassShader.LoadShader((VertexGPShaderPath).c_str(), (FragmentGPShaderPath).c_str());
	}
	else if (type == "Geometry")
	{
		ModelShader.LoadShaderGeom((VertexShaderPath).c_str(), (FragmentShaderPath).c_str(), (GeometryShaderPath).c_str());
		ModelGpassShader.LoadShaderGeom((VertexGPShaderPath).c_str(), (FragmentGPShaderPath).c_str(), (GeometryGPShaderPath).c_str());

		// normal gpass for now
		//ModelGpassShader.LoadShader((VertexGPShaderPath).c_str(), (FragmentGPShaderPath).c_str()); 
	}
	
}

void Material::ClearMaterial()
{
	ModelShader.Delete();
	ModelGpassShader.Delete();
}

void Material::update()
{
	// this is where we activate and send off all the uniforms to the shader
	Camera::Matrix(ModelShader, "camMatrix"); // Send Camera Matrix To Shader Prog

	ModelShader.Activate();
	//uvScale
	ModelGpassShader.Activate();
	Camera::Matrix(ModelGpassShader, "camMatrix"); // Send Camera Matrix To Shader Prog

	updateTime();
}

void Material::updateTime()
{

	ModelShader.Activate();
	ModelShader.setFloat("deltatime", TimeUtil::s_DeltaTime);
	ModelShader.setFloat("time", glfwGetTime());

	ModelGpassShader.Activate();
	ModelGpassShader.setFloat("deltatime", TimeUtil::s_DeltaTime);
	ModelGpassShader.setFloat("time", glfwGetTime());

}

void Material::updateForwardLights() {
	LightingHandler::update(ModelShader);
}	

void Material::jsonLoad(std::string path) 
{
	std::ifstream file(path);
	if (!file.is_open()) {
		std::cout << "Failed to open file: " << path << std::endl;
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
		if (item.contains("GPassGeomShader")) {
			GeometryGPShaderPath = item.at("GPassGeomShader").get<std::string>();
		}



	}
	if (init::LogALL || init::LogModel) std::cout << "Loaded Scene Models from: " << path << std::endl;
}