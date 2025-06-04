#include "ModelObject.h"
#include <utils/VisibilityChecker.h>

std::vector<std::tuple<Model, unsigned int>> ModelObject::loadLODmodelsFromJson(const std::string& jsonFilePath) {
	std::vector<std::tuple<Model, unsigned int>> models;
	std::ifstream file(jsonFilePath);
	if (!file.is_open()) {
		std::cerr << "Failed to open file: " << jsonFilePath << std::endl;
		models.emplace_back(Model("Assets/assets/fallback/model/placeholder/placeholder.gltf"), 0);
		return models;
	}

	json modelData;
	try {
		file >> modelData;
	}
	catch (const std::exception& e) {
		std::cerr << "Error parsing JSON file: " << e.what() << std::endl;
		models.emplace_back(Model("Assets/assets/fallback/model/placeholder/placeholder.gltf"), 0);
		return models;
	}
	file.close();
	unsigned int iteration = 0;
	try {
		LodPath = modelData[0]["LodPath"];
		ModelFileNames = modelData[0]["ModelFileNames"];
		for (int i = 0; i < ModelFileNames.size(); i++)
		{
			iteration = i;
			//std::cout << LodPath + ModelFileNames[i] << " " << iteration << std::endl;
			models.emplace_back(Model(((LodPath + ModelFileNames[i]).c_str())), iteration);
		}
		LodCount = iteration;
	}
	catch (const std::exception& e) {
		std::cerr << "Error processing JSON data (ModelObject): " << e.what() << std::endl;
		models.emplace_back(Model("Assets/assets/fallback/model/placeholder/placeholder.gltf"), 0);
	}

	return models;
}

std::vector<std::tuple<Model>> ModelObject::loadmodelFromJson(const std::string& ModelFilePath) {
	std::vector<std::tuple<Model >> models;
	std::ifstream file(ModelFilePath);
	if (!file.is_open()) {
		std::cerr << "Failed to open file: " << ModelFilePath << std::endl;
		models.emplace_back(Model("Assets/assets/fallback/model/placeholder/placeholder.gltf"));
		return models;
	}

	json modelData;
	try {
		file >> modelData;
	}
	catch (const std::exception& e) {
		std::cerr << "Error parsing JSON file: " << e.what() << std::endl;
		models.emplace_back(Model("Assets/assets/fallback/model/placeholder/placeholder.gltf"));
		return models;
	}
	file.close();
	try {
			models.emplace_back(Model(((ModelFilePath).c_str())));
	}
	catch (const std::exception& e) {
		std::cerr << "Error processing JSON data (ModelObject): " << e.what() << std::endl;
		models.emplace_back(Model("Assets/assets/fallback/model/placeholder/placeholder.gltf"));
	}

	return models;
}


void ModelObject::CreateObject(std::string type, std::string path, std::string ObjectName) {
	if (type == "LOD" || type == "lod" || type == "Lod") {
		CubeCollider.init();
		LodFileReader(path);
		modelOBJ = loadLODmodelsFromJson(path);
		IsLod = true;
	}
	else if (type == "Static" || type == "STATIC") {
		CubeCollider.init();
		SingleModel = loadmodelFromJson(path);
		IsLod = false;
	}
	else {
		std::cout << "ModelObject type: " << type << " not found" << std::endl;
	}

}

void ModelObject::UpdateCollider() {
	switch (isCollider) {
	case true: {
		break;
	}
	case false: {

		break;
	}
	}
	//std::cout << transform.x << " " << transform.y << " " << transform.z << std::endl;
	CubeCollider.colliderXYZ = transform + BoxColliderTransform;
	CubeCollider.colliderScale = BoxColliderScale;
}

void ModelObject::UpdateCameraCollider() {
	CubeCollider.update();
}

unsigned int CalculateLOD(const glm::vec3& cameraPos, const glm::vec3& transform, float LodDistance, unsigned int LodCount) {
	float distance = glm::distance(cameraPos, transform);
	float step = LodDistance / static_cast<float>(LodCount);

	// Clamp the LOD level to ensure it stays within bounds
	unsigned int lodLevel = std::min(LodCount, static_cast<unsigned int>(distance / step));

	return lodLevel;
}


void ModelObject::draw(Shader &Shader) {

	if (DoFrustumCull) {
		if (Camera::isBoxInFrustum((frustumBoxTransform + transform), frustumBoxScale) || VisibilityChecker::isInRange((frustumBoxTransform + transform), Camera::Position, 1 + (Camera::fov * 0.1))) {
			if (DoCulling == true && !ImGuiCamera::isWireframe) { glEnable(GL_CULL_FACE); }
			else { glDisable(GL_CULL_FACE); }

			switch (IsLod) {
			case true: {
				for (auto& modelTuple : modelOBJ) {
					Model& model = std::get<0>(modelTuple);
					unsigned int iteration = std::get<1>(modelTuple);
					//std::cout << CalculateLOD(Camera::Position, transform, LodDistance, LodCount);
					if (iteration == CalculateLOD(Camera::Position, transform, LodDistance, LodCount)) {
						model.Draw(Shader, transform, rotation, scale);
						RenderClass::gPassDraw(model, transform, rotation, scale);
					}
				}
				break;
			}
			case false: {
				for (auto& modelTuple : SingleModel) {
					Model& model = std::get<0>(modelTuple);
					//std::cout << CalculateLOD(Camera::Position, transform, LodDistance, LodCount);
					model.Draw(Shader, transform, rotation, scale);
					RenderClass::gPassDraw(model, transform, rotation, scale);
				}
				break;
			}
			}
			glDisable(GL_CULL_FACE);
		}
	}
	else {
		if (DoCulling == true && !ImGuiCamera::isWireframe) { glEnable(GL_CULL_FACE); }
		else { glDisable(GL_CULL_FACE); }

		switch (IsLod) {
		case true: {
			for (auto& modelTuple : modelOBJ) {
				Model& model = std::get<0>(modelTuple);
				unsigned int iteration = std::get<1>(modelTuple);
				//std::cout << CalculateLOD(Camera::Position, transform, LodDistance, LodCount);
				if (iteration == CalculateLOD(Camera::Position, transform, LodDistance, LodCount)) {
					model.Draw(Shader, transform, rotation, scale);
					RenderClass::gPassDraw(model, transform, rotation, scale);
				}
			}
			break;
		}
		case false: {
			for (auto& modelTuple : SingleModel) {
				Model& model = std::get<0>(modelTuple);
				//std::cout << CalculateLOD(Camera::Position, transform, LodDistance, LodCount);
				model.Draw(Shader, transform, rotation, scale);
				RenderClass::gPassDraw(model, transform, rotation, scale);
			}
			break;
		}
		}
		glDisable(GL_CULL_FACE);
	}
	CubeCollider.draw();
}

void ModelObject::Delete() {

}

void ModelObject::LodFileReader(std::string path) {
	std::ifstream LODConfigFile(path);
	if (LODConfigFile.is_open()) {
		json LODConfigData;
		LODConfigFile >> LODConfigData;
		LODConfigFile.close();

		
		LodDistance = LODConfigData[0]["LodDistance"];
		DoCulling = LODConfigData[0]["DoCulling"];
		
		BoxColliderTransform.x = LODConfigData[0]["LocalBoxColliderLocation"][0];
		BoxColliderTransform.y = LODConfigData[0]["LocalBoxColliderLocation"][1];
		BoxColliderTransform.z = LODConfigData[0]["LocalBoxColliderLocation"][2];

		BoxColliderScale.x = LODConfigData[0]["LocalBoxColliderSize"][0];
		BoxColliderScale.y = LODConfigData[0]["LocalBoxColliderSize"][1];
		BoxColliderScale.z = LODConfigData[0]["LocalBoxColliderSize"][2];


	}
	else {
		std::cerr << "Failed to open LODConfig: " << path << std::endl;
	}
}