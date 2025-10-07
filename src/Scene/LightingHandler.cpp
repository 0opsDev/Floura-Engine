#include "LightingHandler.h"
#include <utils/init.h>
#include <Core/Render.h>
#include <glm/gtx/euler_angles.hpp>
#include <utils/logConsole.h>

std::vector<LightingHandler::Light> LightingHandler::Lights;

// DIR light
glm::vec3 LightingHandler::dirLightRot = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 LightingHandler::directLightCol = glm::vec3(1.0f, 1.0f, 1.0f);
float LightingHandler::directAmbient = 0.20f;
float LightingHandler::dirSpecularLight = 0.20f;
bool LightingHandler::doDirLight = false;
bool LightingHandler::doDirSpecularLight = true;

void LightingHandler::update(Shader Shader)
{
	Shader.Activate();

	Shader.setFloat4("skyColor", RenderClass::skyRGBA.r, RenderClass::skyRGBA.g, RenderClass::skyRGBA.b, 1.0f);
	Shader.setBool("doReflect", RenderClass::doReflections);


	int activeLightIndex = 0;

	for (size_t i = 0; i < Lights.size(); ++i)
	{
		if (Lights[i].enabled)
		{
			std::string uniformName = "Lights[" + std::to_string(activeLightIndex) + "].";
			Shader.setFloat3((uniformName + "position").c_str(), Lights[i].position.x, Lights[i].position.y, Lights[i].position.z);

			glm::vec3 baseDirection = glm::vec3(0, 0, -1);
			glm::vec3 eulerDegrees = Lights[i].rotation;
			glm::vec3 eulerRadians = glm::radians(eulerDegrees);
			glm::mat4 rotationMatrix = glm::yawPitchRoll(eulerRadians.y, eulerRadians.x, eulerRadians.z);
			glm::vec3 rotatedDirection = glm::vec3(rotationMatrix * glm::vec4(baseDirection, 0.0f));

			Shader.setFloat3((uniformName + "rotation").c_str(), rotatedDirection.x, rotatedDirection.y, rotatedDirection.z);


			Shader.setFloat3((uniformName + "colour").c_str(), Lights[i].colour.x, Lights[i].colour.y, Lights[i].colour.z);
			Shader.setFloat((uniformName + "radius").c_str(), Lights[i].radius);
			Shader.setInt((uniformName + "type").c_str(), Lights[i].type);

			activeLightIndex++;
		}

	}

	Shader.setFloat("NearPlane", RenderClass::DepthPlane[0]);
	Shader.setFloat("FarPlane", RenderClass::DepthPlane[1]);
	Shader.setInt("lightCount", activeLightIndex);

	// directional light
	glm::vec3 baseDirectionDIR = glm::vec3(0, 0, -1);
	glm::vec3 eulerDegreesDIR = dirLightRot;
	glm::vec3 eulerRadiansDIR = glm::radians(eulerDegreesDIR);
	glm::mat4 rotationMatrixDIR = glm::yawPitchRoll(eulerRadiansDIR.y, eulerRadiansDIR.x, eulerRadiansDIR.z);
	glm::vec3 rotatedDirectionDIR = glm::vec3(rotationMatrixDIR * glm::vec4(baseDirectionDIR, 0.0f));

	Shader.setBool("doDirLight", doDirLight);
	Shader.setBool("doDirSpecularLight", doDirSpecularLight);
	Shader.setFloat("directAmbient", directAmbient);
	Shader.setFloat("dirSpecularLight", dirSpecularLight);
	Shader.setFloat3("directLightPos", rotatedDirectionDIR.x, rotatedDirectionDIR.y, rotatedDirectionDIR.z); // 0.0f, 1.0f, 0.0f
	Shader.setFloat3("directLightCol", directLightCol.x, directLightCol.y, directLightCol.z); // 1.0f, 1.0f, 1.0f
}

void LightingHandler::createLight()
{
	Light tempLight;

	tempLight.position = glm::vec3(0);
	tempLight.rotation = glm::vec3(0);
	tempLight.colour = glm::vec3(1);
	tempLight.radius = 100.0f;
	tempLight.type = 0;
	tempLight.enabled = true;

	Lights.push_back(tempLight);

	LogConsole::print("Created LightObject");
}

void LightingHandler::loadScene(std::string& path)
{
	LightingHandler::deleteScene();

	std::ifstream file(path);
	if (!file.is_open()) {
		std::cout << "Failed to open file: " << path << std::endl;
		return;
	}
	json LightObjectFileData;
	try {
		file >> LightObjectFileData;
	}
	catch (const nlohmann::json::parse_error& e) {
		// This catch block specifically handles JSON parsing errors,
		// which gives more precise error information from the library.
		std::cout << "JSON Parse Error loading LightObject data: " << e.what() << std::endl;
		std::cout << "Error byte position: " << e.byte << std::endl; // Specific to nlohmann::json
	}
	catch (const std::ios_base::failure& e) {
		// This catch block handles file I/O errors (e.g., file not found, permission issues).
		std::cout << "File I/O Error loading LightObject data: " << e.what() << std::endl;
	}
	catch (const std::exception& e) {
		// A general catch-all for any other std::exception derived errors.
		std::cout << "An unexpected error occurred loading LightObject data: " << e.what() << std::endl;
	}
	file.close();

	for (const auto& item : LightObjectFileData) {

		glm::vec3 newPosition = glm::vec3(item.at("position")[0], item.at("position")[1], item.at("position")[2]);
		glm::vec3 newRotation = glm::vec3(item.at("rotation")[0], item.at("rotation")[1], item.at("rotation")[2]);
		glm::vec3 newColour = glm::vec3(item.at("colour")[0], item.at("colour")[1], item.at("colour")[2]);
		float radius = item.at("radius");
		std::string newType = item.at("type").get<std::string>();
		bool newEnabled = item.at("enabled").get<bool>();

		Light tempLight;

		tempLight.position = newPosition;
		tempLight.rotation = newRotation;
		tempLight.colour = newColour;
		tempLight.radius = radius;

		if (newType == "pointlight" || newType == "Pointlight") {
			tempLight.type = 1;
		}
		else if (newType == "spotlight" || newType == "Spotlight") {
			tempLight.type = 0;
		}

		tempLight.enabled = newEnabled;

		Lights.push_back(tempLight);


		// pushback here
	}
	if (init::LogALL || init::LogModel) std::cout << "Loaded Scene LightObject from: " << path << std::endl;
}

void LightingHandler::saveScene(std::string& path)
{
	try {
		json lightData = json::array();  // New JSON array to hold model data

		int iteration = 0;
		// Serialize each modelObject into JSON
		for (const auto& Write : LightingHandler::Lights) {
			json LightJson;

			LightJson["position"] = { LightingHandler::Lights[iteration].position.x,  LightingHandler::Lights[iteration].position.y,  LightingHandler::Lights[iteration].position.z };
			LightJson["rotation"] = { LightingHandler::Lights[iteration].rotation.x,  LightingHandler::Lights[iteration].rotation.y,  LightingHandler::Lights[iteration].rotation.z };
			LightJson["colour"] = { LightingHandler::Lights[iteration].colour.x, LightingHandler::Lights[iteration].colour.y, LightingHandler::Lights[iteration].colour.z };

			if (LightingHandler::Lights[iteration].type == 1) {
				LightJson["type"] = "pointlight";
			}
			else if (LightingHandler::Lights[iteration].type == 0) {
				LightJson["type"] = "spotlight";
			}

			LightJson["radius"] = LightingHandler::Lights[iteration].radius;

			LightJson["enabled"] = LightingHandler::Lights[iteration].enabled;

			lightData.push_back(LightJson);
			iteration++;
		}

		// Write to file
		std::ofstream outFile(path, std::ios::out);
		if (!outFile.is_open()) {
			if (init::LogALL || init::LogSystems) std::cout << "Failed to write to " << path << std::endl;
			return;
		}

		outFile << lightData.dump(4);  // Pretty-print with indentation
		outFile.close();

		if (init::LogALL || init::LogSystems) std::cout << "Successfully updated " << path << std::endl;

	}
	catch (const std::exception& e) {
		if (init::LogALL || init::LogSystems) std::cout << "Exception: " << e.what() << std::endl;
	}
}

void LightingHandler::deleteScene()
{
	Lights.clear();
}