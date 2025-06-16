#ifndef MODEL_OBJECT_CLASS_H
#define MODEL_OBJECT_CLASS_H
#include "Physics/CubeCollider.h"
#include <glm/ext/quaternion_float.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#include <string>
#include <iostream>
#include "Render/Cube/Billboard.h"
#include"Render/Model/Model.h"
#include <camera/Camera.h>
#include <Render/Cube/CubeVisualizer.h>
#include <Render/Shader/shaderClass.h>
#include <json/json.hpp>
#include <utility>
#include <stdexcept>
#include<fstream>
#include<sstream>
#include<cerrno>
#include <utils/init.h>
#include <glm/gtx/string_cast.hpp>
#include <UI/ImGui/ImGuiWindow.h>
using json = nlohmann::json;

class ModelObject
{
public:
	CubeCollider CubeCollider;
	std::vector<std::string>ModelFileNames;
	bool DoCulling = true;
	std::string ObjectName;
	bool hasPhysics = false;
	float LodDistance = 100.0f;
	unsigned int LodCount;
	glm::vec3 transform = glm::vec3(0, 0, 0);
	glm::vec3 scale = glm::vec3(1, 1, 1);
	glm::vec4 rotation = glm::vec4(0,0,0,0);

	bool isCollider = false;
	glm::vec3 BoxColliderTransform = glm::vec3(0, 0, 0);
	glm::vec3 BoxColliderScale = glm::vec3(1, 1, 1);

	bool DoFrustumCull = false;
	glm::vec3 frustumBoxTransform = glm::vec3(0, 0, 0);
	glm::vec3 frustumBoxScale = glm::vec3(1, 1, 1);

	void CreateObject(std::string type, std::string path, std::string ObjectName); // type LOD or Regular

	void UpdateCollider();

	void UpdateCameraCollider();

	void draw(Shader& Shader);

	void Delete();

private:
	bool IsLod;
	std::string LodPath;
	void LodFileReader(std::string path);

	std::vector<std::tuple<Model, unsigned int>> loadLODmodelsFromJson(const std::string& jsonFilePath);
	std::vector<std::tuple<Model>> loadmodelFromJson(const std::string& ModelFilePath);

	std::vector<std::tuple<Model>> SingleModel;
	std::vector<std::tuple<Model, unsigned int>> modelOBJ;
	void renderLogic(Shader& Shader);
	
};

#endif // MODEL_OBJECT_CLASS_H