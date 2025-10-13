#include "LightingHandler.h"
#include <utils/init.h>
#include <Core/Render.h>
#include <glm/gtx/euler_angles.hpp>
#include <utils/logConsole.h>

std::vector<LightingHandler::Light> LightingHandler::Lights;

// DIR light
glm::vec3 LightingHandler::dirLightRot = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 LightingHandler::dirLightPosOut = glm::vec3(0.0f, 1.0f, 0.0f);;
glm::vec3 LightingHandler::directLightCol = glm::vec3(1.0f, 1.0f, 1.0f);
float LightingHandler::directAmbient = 0.20f;
float LightingHandler::dirSpecularLight = 0.20f;
bool LightingHandler::doDirLight = false;
bool LightingHandler::doDirSpecularLight = true;

unsigned int LightingHandler::shadowMapFBO, LightingHandler::shadowMapHeight, LightingHandler::shadowMapWidth, LightingHandler::ShadowMap;
float LightingHandler::distance = 35.0f;
glm::vec2 LightingHandler::dirNearFar = glm::vec2(0.1f, 75.0f); // 0.1f 75.0f
float LightingHandler::dirShadowheight = 20.0f;
bool LightingHandler::doDirShadowMap = true;

glm::mat4 LightingHandler::lightProjection;
Shader LightingHandler::dirShadowMapProgram;

void LightingHandler::setupShadowMapBuffer() {
	LightingHandler::dirShadowMapProgram.LoadShader("Shaders/Lighting/shadowMap.vert", "Shaders/Lighting/shadowMap.frag");
	//shadowMapHeight = 4096;
	//shadowMapWidth = 4096;
	//shadowMapHeight = 2046;
	//shadowMapWidth = 2046;
	shadowMapHeight = 1024;
	shadowMapWidth = 1024;
	glGenFramebuffers(1, &shadowMapFBO);
	glGenTextures(1, &ShadowMap);
	glBindTexture(GL_TEXTURE_2D, ShadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapWidth, shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float clampColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clampColor);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, ShadowMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void LightingHandler::drawShadowMap(Model model, glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale) {
	if (!doDirShadowMap)
	{
		return;
	}
	glm::mat4 orthgonalProjection = glm::ortho(-distance, distance, -distance, distance, dirNearFar.x, dirNearFar.y); // last two are near and far 

	glm::vec3 CameraPos = Camera::Position;
	glm::vec3 dirLightDirection = dirLightPosOut;
	glm::vec3 lightEyePosition = CameraPos + (dirShadowheight * dirLightDirection);

	glm::mat4 lightView = glm::lookAt(
		lightEyePosition,
		CameraPos,
		glm::vec3(0.0f, 1.0f, 0.0f)
	);
	lightProjection = orthgonalProjection * lightView;
	//glm::mat4 lightView = glm::lookAt(20.0f * LightingHandler::dirLightPosOut, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//LightingHandler::lightProjection = orthgonalProjection * lightView;


	glEnable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glViewport(0, 0, shadowMapWidth, shadowMapHeight); // dont touch for now
	//glClear(GL_DEPTH_BUFFER_BIT);
	LightingHandler::dirShadowMapProgram.Activate();
	glUniformMatrix4fv(glGetUniformLocation(LightingHandler::dirShadowMapProgram.ID, "lightProjection"), 1, GL_FALSE, glm::value_ptr(lightProjection));

	model.Draw(LightingHandler::dirShadowMapProgram, translation, rotation, scale);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, Camera::width, Camera::height); // dont touch for now

}

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

	dirLightPosOut = rotatedDirectionDIR;

	Shader.setBool("doDirLight", doDirLight);
	Shader.setBool("doDirSpecularLight", doDirSpecularLight);
	Shader.setFloat("directAmbient", directAmbient);
	Shader.setFloat("dirSpecularLight", dirSpecularLight);
	Shader.setFloat3("directLightPos", rotatedDirectionDIR.x, rotatedDirectionDIR.y, rotatedDirectionDIR.z); // 0.0f, 1.0f, 0.0f
	Shader.setFloat3("directLightCol", directLightCol.x, directLightCol.y, directLightCol.z); // 1.0f, 1.0f, 1.0f

	Shader.setFloat("doDirShadowMap", doDirShadowMap);
	if (doDirShadowMap)
	{
		// shadow map
		glUniformMatrix4fv(glGetUniformLocation(Shader.ID, "lightProjection"), 1, GL_FALSE, glm::value_ptr(lightProjection));

		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_2D, ShadowMap);
		glUniform1i(glGetUniformLocation(Shader.ID, "shadowMap"), 2);
	}
	
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

	tempLight.ID.ObjType = 'l';
	tempLight.ID.index = LightingHandler::Lights.size();
	IdManager::AddID(tempLight.ID);

	Lights.push_back(tempLight);

	LogConsole::print("Created LightObject");
}

void LightingHandler::deleteLight(int index)
{
	//update lowest free index
	if (index < IdManager::lowestDeletedIndex.Light || IdManager::lowestDeletedIndex.Light == -1) {
		IdManager::lowestDeletedIndex.Light = index;
		LogConsole::print("Lowest Deleted LightObject Index is now: " + std::to_string(IdManager::lowestDeletedIndex.Light));
	}
	if (index < 0 || index >= static_cast<int>(Lights.size())) {
		std::cout << "Invalid index: " << index << std::endl;
		return;
	}
	IdManager::RemoveID(Lights[index].ID);
	Lights.erase(Lights.begin() + index);
	if (init::LogALL || init::LogModel) LogConsole::print("Deleted LightObject at index: " + std::to_string(index));

	//IdManager::lowestLightIndexSync(); // sync up the index after deletion because the array has now changed
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
		tempLight.ID.ObjType = 'l';
		tempLight.ID.UniqueNumber = item.at("IDuniqueIdentifier").get<unsigned int>();
		if (init::LogALL || init::LogModel) LogConsole::print("Loaded LightObject with ID: " + std::to_string(tempLight.ID.UniqueNumber));
		tempLight.ID.index = LightingHandler::Lights.size();
		IdManager::AddID(tempLight.ID);

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
			// ID
			LightJson["IDuniqueIdentifier"] = LightingHandler::Lights[iteration].ID.UniqueNumber;

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
	for (size_t i = 0; i < Lights.size(); ++i)
	{
		deleteLight(i);
		//IdManager::RemoveID(Lights[i].ID);
	}
	Lights.clear();
}