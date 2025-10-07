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
#include <Render/Shader/Material.h>
using json = nlohmann::json;

class ModelObject
{
public:

	Material MaterialObject;

	//std::vector <Shader> ModelShaders;
	//std::vector <Shader> ModelGpassShaders;

	CubeCollider CubeCollider;
	std::vector<std::string>ModelFileNames;
	bool DoCulling = true;
	std::string ObjectName;
	bool hasPhysics = false;
	float LodDistance = 100.0f;
	unsigned int LodCount;
	glm::vec3 transform = glm::vec3(0, 0, 0);
	glm::vec3 scale = glm::vec3(1, 1, 1);
	glm::vec3 rotation = glm::vec3(0,0,0);
	bool CullFrontFace = false;

	bool isCollider = false;
	glm::vec3 BoxColliderTransform = glm::vec3(0, 0, 0);
	glm::vec3 BoxColliderScale = glm::vec3(1, 1, 1);

	bool DoFrustumCull = false;
	glm::vec3 frustumBoxTransform = glm::vec3(0, 0, 0);
	glm::vec3 frustumBoxScale = glm::vec3(1, 1, 1);

	std::string ModelPath;

	// flag
	bool IsLod;

	void LoadMaterial(std::string path);

	void CreateObject(std::string path, std::string ObjectNameT, std::string Material); // type LOD or Regular

	void UpdateCollider();

	void UpdateCameraCollider();

	void updateForwardLights();

	void draw();

	void Delete();

	Model ModelSingle; // should be private but im leaving it here just for now
	std::vector<Model> LODModels;
private:
	
	std::string LodPath;

	void LODModelLoad(std::string path);
	void SingleModelLoad(std::string path);
	
	//std::vector<std::tuple<Model, unsigned int>> loadLODmodelsFromJson(const std::string& jsonFilePath);
	//std::vector<std::tuple<Model>> loadmodelFromJson(const std::string& ModelFilePath);

	//std::vector<std::tuple<Model>> SingleModel;
	//std::vector<std::tuple<Model, unsigned int>> modelOBJ;
	void renderLogic();
	
};

#endif // MODEL_OBJECT_CLASS_H