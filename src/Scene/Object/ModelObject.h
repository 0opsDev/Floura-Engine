#ifndef MODEL_OBJECT_CLASS_H
#define MODEL_OBJECT_CLASS_H
#include "Physics/CubeCollider.h"
#include <glm/ext/quaternion_float.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#include <string>
#include <iostream>
#include "Render/Cube/Billboard.h"
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
#include <Scene/IdManager.h>
#include <Render/Model/ModelAssimp.h>
using json = nlohmann::json;

class ModelObject
{
public:

	Material MaterialObject;
	IdManager::ID ID;

	//std::vector <Shader> ModelShaders;
	//std::vector <Shader> ModelGpassShaders;

	CubeCollider CubeCollider;
	std::vector<std::string>ModelFileNames;
	bool DoCulling = true;
	std::string ObjectName;
	bool castShadow = true;
	float LodDistance = 100.0f;
	unsigned int LodCount;
	glm::vec3 transform = glm::vec3(0, 0, 0);
	glm::vec3 scale = glm::vec3(1, 1, 1);
	glm::vec3 rotation = glm::vec3(0,0,0);
	bool CullFrontFace = false;

	bool isCollider = false;
	glm::vec3 BoxColliderTransform = glm::vec3(0, 0, 0);
	glm::vec3 BoxColliderScale = glm::vec3(1, 1, 1);

	std::string ModelPath;

	glm::vec2 uvScale = glm::vec2(1.0f, 1.0f);

	// flag
	bool IsLod;

	void LoadMaterial(std::string path);

	void CreateObject(std::string path, std::string ObjectNameT, std::string Material); // type LOD or Regular

	void UpdateCollider();

	void UpdateCameraCollider();

	void updateForwardLights();

	void draw();

	void drawModelShadowMap();

	void Delete();

	aModel ModelSingle; // should be private but im leaving it here just for now
	std::vector<aModel> LODModels;
private:
	
	std::string LodPath;

	void LODModelLoad(std::string path);
	void SingleModelLoad(std::string path);
	void renderLogic(bool shadowmap);
	
};

#endif // MODEL_OBJECT_CLASS_H