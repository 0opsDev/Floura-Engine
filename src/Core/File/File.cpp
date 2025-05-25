#include "File.h"
std::pair<std::string, std::string> FileClass::getShaderPaths(int vertIndex, int fragIndex) {
	std::ifstream file("Shaders/ShaderList.json"); // turn into string
	if (!file.is_open()) {
		throw std::runtime_error("Failed to open file: Shaders/ShaderList.json");
	}

	json shaderData;
	file >> shaderData;
	file.close();

	std::string vertPath = shaderData[0]["Vert"].at(vertIndex); //check for paths (strings) in array at number index givin and return it
	std::string fragPath = shaderData[0]["Frag"].at(fragIndex);

	return { vertPath, fragPath };
}

std::vector<std::tuple<Model, int, glm::vec3, glm::quat, glm::vec3, int>> FileClass::loadModelsFromJson(const std::string& jsonFilePath) {
	std::vector<std::tuple<Model, int, glm::vec3, glm::quat, glm::vec3, int>> models;
	std::ifstream file(jsonFilePath);
	if (!file.is_open()) {
		std::cerr << "Failed to open file: " << jsonFilePath << std::endl;
		models.emplace_back(Model("Assets/assets/fallback/model/placeholder/placeholder.gltf"), 0, glm::vec3(0.0f, 0.0f, 0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 0);
		return models;
	}

	json modelData;
	try {
		file >> modelData;
	}
	catch (const std::exception& e) {
		std::cerr << "Error parsing JSON file: " << e.what() << std::endl;
		models.emplace_back(Model("Assets/assets/fallback/model/placeholder/placeholder.gltf"), 0, glm::vec3(0.0f, 0.0f, 0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 0);
		return models;
	}
	file.close();

	try {
		for (const auto& item : modelData) {
			std::string name = item.at("name").get<std::string>();
			std::string path = item.at("path").get<std::string>();
			bool isCulling = item.at("isCulling").get<bool>();
			glm::vec3 location = glm::vec3(item.at("Location")[0], item.at("Location")[1], item.at("Location")[2]);
			glm::quat rotation = glm::quat(item.at("Rotation")[0], item.at("Rotation")[1], item.at("Rotation")[2], item.at("Rotation")[3]);
			glm::vec3 scale = glm::vec3(item.at("Scale")[0], item.at("Scale")[1], item.at("Scale")[2]);
			bool isCollider = item.at("isCollider").get<bool>();
			models.emplace_back(Model(("Assets/assets/" + path).c_str()), isCulling, location, rotation, scale, isCollider);
			if (init::LogALL || init::LogModel) std::cout << "Loaded model: " << '"' << name << '"' << " from path: " << path << " at location: " << glm::to_string(location) << std::endl;
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Error processing JSON data: " << e.what() << std::endl;
		models.emplace_back(Model("Assets/assets/fallback/model/placeholder/placeholder.gltf"), 0, glm::vec3(0.0f, 0.0f, 0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 0);
	}

	return models;
}

void FileClass::loadShaderProgram(int VertNum, int FragNum, Shader& shaderProgram) { //shader program switcher

	shaderProgram.Delete();

	try {
		std::pair<std::string, std::string> shaderPaths = FileClass::getShaderPaths(VertNum, FragNum);
		std::string vertFile = shaderPaths.first;
		std::string fragFile = shaderPaths.second;

		if (init::LogALL || init::LogSystems) std::cout << "Vert: " << vertFile << " Frag: " << fragFile << std::endl;

		shaderProgram.LoadShader(vertFile.c_str(), fragFile.c_str());
	}
	catch (const std::exception& e) {
		std::cerr << "Error loading shader program: " << e.what() << std::endl;
	}
}