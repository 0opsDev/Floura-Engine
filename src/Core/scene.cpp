#include "scene.h"
#include <Render/window/WindowHandler.h>
#include <Gameplay/Player.h>
#include <utils/VisibilityChecker.h>

//BillBoardObject BillBoardObjects;
//CubeCollider ColliderObject;
//SoundProgram SoundObjects;
//std::vector<ModelObject> Scene::objects;

std::vector<std::unique_ptr<ModelObject>> Scene::modelObjects;
std::vector<BillBoardObject> Scene::BillBoardObjects;
std::vector <CubeCollider> Scene::CubeColliderObject;
std::vector <SoundProgram> Scene::SoundObjects;
std::vector <bool> Scene::isSoundLoop;

std::vector<glm::vec3> Scene::colour;
std::vector<glm::vec3> Scene::position;
std::vector<glm::vec2> Scene::radiusAndPower;
std::vector<int> Scene::lightType;
std::vector<int> Scene::enabled;

glm::vec3 Scene::initalCameraPos = glm::vec3(0, 0, 0);


BillBoard PointLightIcon;
BillBoard SpotLightIcon;

void Scene::init() {
	PointLightIcon.init("Assets/Icons/point.png");
	SpotLightIcon.init("Assets/Icons/spot.png");
}

void Scene::LoadScene(std::string path) {

	// Attemp to delete previous scene
	Delete();

	if (FEImGuiWindow::imGuiEnabled) {
		FEImGuiWindow::loadContentObjects(path + "/ContentObject.scene");
	}

	initJsonSettingsLoad(path + "/Settings.scene");
	initCameraSettingsLoad(path + "/Camera.scene");
	initJsonColliderLoad(path + "/Collider.scene");
	initJsonLightObjectLoad(path + "/Lights.scene");
	initJsonBillBoardLoad(path + "/BillBoard.scene");
	initJsonModelLoad(path + "/Model.scene");
	initJsonSoundObjectLoad(path + "/Sound.scene");
	/*
		for (size_t i = 0; i < modelObjects.size(); i++) {
		std::cout << modelObjects[i].ObjectName << " - " << modelObjects[i].ModelPath << std::endl;
		std::cout << modelObjects[i].ModelSingle.meshes.size() << " - indmesh size" << std::endl;

		
		//.vertices.size()
		for (size_t z = 0; z < modelObjects[i].ModelSingle.meshes.size(); z++)
		{

						std::cout << modelObjects[i].ModelSingle.meshes[z].vertices.size() << " - indmesh vertices size" << std::endl;
					for (size_t y = 0; y < modelObjects[i].ModelSingle.meshes[z].vertices.size(); y++)
			{
				std::cout << modelObjects[i].ModelSingle.meshes[z].vertices[y].position.x << " - vertex position x" << std::endl;
				std::cout << modelObjects[i].ModelSingle.meshes[z].vertices[y].position.x << " - vertex position y" << std::endl;
				std::cout << modelObjects[i].ModelSingle.meshes[z].vertices[y].position.x << " - vertex position z" << std::endl;
			}
			//std::cout << modelObjects[i].ModelSingle.meshes[i].vertices << " - vertex" << std::endl;

		}
	*/

}


void Scene::SaveScene(std::string path) {

	if (FEImGuiWindow::imGuiEnabled) {
		FEImGuiWindow::saveContentObjects(path + "/ContentObject.scene");
	}

	JsonSettingsSave(path + "/Settings.scene");
	JsonCameraSettingsSave(path + "/Camera.scene");
	JsonColliderSave(path + "/Collider.scene");
	JsonLightSave(path + "/Lights.scene");
	JsonBillBoardSave(path + "/BillBoard.scene");
	JsonModelSave(path + "/Model.scene");
	// sound load here

}

void Scene::initJsonModelLoad(std::string path) {
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
		std::unique_ptr<ModelObject> newObject = std::make_unique<ModelObject>(); // Use std::make_unique

		std::string name = item.at("name").get<std::string>();
		std::string type = item.at("type").get<std::string>();
		std::string path = item.at("path").get<std::string>();
		std::string MaterialPath = item.at("MaterialPath").get<std::string>();


		glm::vec3 Location = glm::vec3(item.at("Location")[0], item.at("Location")[1], item.at("Location")[2]);
		glm::vec3 rotation = glm::vec3(item.at("Rotation")[0], item.at("Rotation")[1], item.at("Rotation")[2]);
		glm::vec3 scale = glm::vec3(item.at("Scale")[0], item.at("Scale")[1], item.at("Scale")[2]);

		glm::vec3 frustumBoxTransform = glm::vec3(item.at("frustumBoxTransform")[0],
			item.at("frustumBoxTransform")[1], item.at("frustumBoxTransform")[2]);
		glm::vec3 frustumBoxScale = glm::vec3(item.at("frustumBoxScale")[0],
			item.at("frustumBoxScale")[1], item.at("frustumBoxScale")[2]);

		glm::vec3 BoxColliderTransform = glm::vec3(item.at("BoxColliderTransform")[0],
			item.at("BoxColliderTransform")[1], item.at("BoxColliderTransform")[2]);
		glm::vec3 BoxColliderScale = glm::vec3(item.at("BoxColliderScale")[0],
			item.at("BoxColliderScale")[1], item.at("BoxColliderScale")[2]);

		bool isCollider = item.at("isCollider").get<bool>();
		bool isBackFaceCulling = item.at("isBackFaceCulling").get<bool>();
		bool DoFrustumCull = item.at("DoFrustumCull").get<bool>();

		newObject->CreateObject(type, path, name, MaterialPath); // Load into this unique MaterialObject
		newObject->transform = Location;
		newObject->rotation = rotation;
		newObject->scale = scale;
		newObject->isCollider = isCollider;
		newObject->DoCulling = isBackFaceCulling;
		newObject->DoFrustumCull = DoFrustumCull;
		newObject->BoxColliderTransform = BoxColliderTransform;
		newObject->BoxColliderScale = BoxColliderScale;
		newObject->frustumBoxTransform = frustumBoxTransform;
		newObject->frustumBoxScale = frustumBoxScale;

		/*
		we make a unique modelobject with (std::make_unique) instead of cloning, then we point to the new unique modelobject  in memory within the array with std::move,
		which stops it from making a bit to bit clone of the prior object, and leaving the shaderprogram ID behind.
		it generates a model object then makes the index of the array point to the new unique object in memory, which doesnt have its own copied id
		*/
		modelObjects.push_back(std::move(newObject));

	}
	if (init::LogALL || init::LogModel) std::cout << "Loaded Scene Models from: " << path << std::endl;
}

void Scene::JsonModelSave(std::string path) {
	try {
		json settingsData = json::array();  // New JSON array to hold model data

		for (const auto& obj : modelObjects) {
			json modelJson;
			modelJson["name"] = obj->ObjectName;

			std::string isLOD;
			if (obj->IsLod) isLOD = "LOD";
			else isLOD = "Static";
			modelJson["type"] = isLOD;

			modelJson["path"] = obj->ModelPath;

			modelJson["Location"] = { obj->transform.x, obj->transform.y, obj->transform.z };
			modelJson["Rotation"] = { obj->rotation.x, obj->rotation.y, obj->rotation.z};
			modelJson["Scale"] = { obj->scale.x, obj->scale.y, obj->scale.z };

			modelJson["frustumBoxTransform"] = { obj->frustumBoxTransform.x, obj->frustumBoxTransform.y, obj->frustumBoxTransform.z };
			modelJson["frustumBoxScale"] = { obj->frustumBoxScale.x, obj->frustumBoxScale.y, obj->frustumBoxScale.z };

			modelJson["BoxColliderTransform"] = { obj->BoxColliderTransform.x, obj->BoxColliderTransform.y, obj->BoxColliderTransform.z };
			modelJson["BoxColliderScale"] = { obj->BoxColliderScale.x, obj->BoxColliderScale.y, obj->BoxColliderScale.z };

			modelJson["isCollider"] = obj->isCollider;
			modelJson["isBackFaceCulling"] = obj->DoCulling;
			modelJson["DoFrustumCull"] = obj->DoFrustumCull;
			modelJson["MaterialPath"] = obj->MaterialObject.materialPath;

			settingsData.push_back(modelJson);
		}
		// Serialize each modelObject into JSON
		

		// Write to file
		std::ofstream outFile(path, std::ios::out);
		if (!outFile.is_open()) {
			if (init::LogALL || init::LogSystems) std::cout << "Failed to write to " << path << std::endl;
			return;
		}

		outFile << settingsData.dump(4);  // Pretty-print with indentation
		outFile.close();

		if (init::LogALL || init::LogSystems) std::cout << "Successfully updated " << path << std::endl;

	}
	catch (const std::exception& e) {
		if (init::LogALL || init::LogSystems) std::cout << "Exception: " << e.what() << std::endl;
	}
}

void Scene::JsonBillBoardSave(std::string path) {
	try {
		json settingsData = json::array();  // New JSON array to hold model data

		// Serialize each modelObject into JSON
		for (const auto& obj : BillBoardObjects) {
			json BillBoardJson;
			BillBoardJson["name"] = obj.ObjectName;
			BillBoardJson["type"] = obj.type;
			BillBoardJson["path"] = obj.path;

			BillBoardJson["doPitch"] = obj.doPitch;
			BillBoardJson["isCollider"] = obj.isCollider;
			BillBoardJson["DoFrustumCull"] = obj.DoFrustumCull;

			if (obj.type == "animated" || obj.type == "Animated") {
				BillBoardJson["doUpdateSequence"] = obj.doUpdateSequence;
				BillBoardJson["tickrate"] = obj.tickrate;
			}

			BillBoardJson["position"] = { obj.transform.x, obj.transform.y, obj.transform.z };
			BillBoardJson["scale"] = { obj.scale.x, obj.scale.y, obj.scale.z };


			settingsData.push_back(BillBoardJson);
		}

		// Write to file
		std::ofstream outFile(path, std::ios::out);
		if (!outFile.is_open()) {
			if (init::LogALL || init::LogSystems) std::cout << "Failed to write to " << path << std::endl;
			return;
		}

		outFile << settingsData.dump(4);  // Pretty-print with indentation
		outFile.close();

		if (init::LogALL || init::LogSystems) std::cout << "Successfully updated " << path << std::endl;

	}
	catch (const std::exception& e) {
		if (init::LogALL || init::LogSystems) std::cout << "Exception: " << e.what() << std::endl;
	}

}

void Scene::JsonColliderSave(std::string path) {
	try {
		json ColliderData = json::array();  // New JSON array to hold model data

		// Serialize each modelObject into JSON
		for (const auto& obj : CubeColliderObject) {
			json CubeColliderJson;
			CubeColliderJson["name"] = obj.name;

			CubeColliderJson["enabled"] = obj.enabled;

			CubeColliderJson["position"] = { obj.colliderXYZ.x, obj.colliderXYZ.y, obj.colliderXYZ.z };
			CubeColliderJson["scale"] = { obj.colliderScale.x, obj.colliderScale.y, obj.colliderScale.z };


			ColliderData.push_back(CubeColliderJson);
		}

		// Write to file
		std::ofstream outFile(path, std::ios::out);
		if (!outFile.is_open()) {
			if (init::LogALL || init::LogSystems) std::cout << "Failed to write to " << path << std::endl;
			return;
		}

		outFile << ColliderData.dump(4);  // Pretty-print with indentation
		outFile.close();

		if (init::LogALL || init::LogSystems) std::cout << "Successfully updated " << path << std::endl;

	}
	catch (const std::exception& e) {
		if (init::LogALL || init::LogSystems) std::cout << "Exception: " << e.what() << std::endl;
	}
}

void Scene::JsonLightSave(std::string path) {
	try {
		json lightData = json::array();  // New JSON array to hold model data

		int iteration = 0;
		// Serialize each modelObject into JSON
		for (const auto& Write : enabled) {
			json LightJson;

			LightJson["position"] = { position[iteration].x, position[iteration].y, position[iteration].z };
			LightJson["colour"] = { colour[iteration].x, colour[iteration].y, colour[iteration].z };

			if (enabled[iteration] > 0) {
				LightJson["enabled"] = true;
			}
			else{
				LightJson["enabled"] = false;
			}

			if (lightType[iteration] == 1) {
				LightJson["type"] = "pointlight";
			}
			else if (lightType[iteration] == 0) {
				LightJson["type"] = "spotlight";
			}

			

			LightJson["radius"] = radiusAndPower[iteration].x;
			LightJson["power"] = radiusAndPower[iteration].y;

			//LightJson["type"] = lightType[iteration];
			
			
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

void Scene::JsonSettingsSave(std::string path) {
	try {
		json SettingsData = json::array();  // New JSON array to hold model data

		json JsonSettings;

		JsonSettings["skyRGBA"][0] = RenderClass::skyRGBA[0];
		JsonSettings["skyRGBA"][1] = RenderClass::skyRGBA[1];
		JsonSettings["skyRGBA"][2] = RenderClass::skyRGBA[2];

		JsonSettings["fogRGBA"][0] = RenderClass::fogRGBA[0];
		JsonSettings["fogRGBA"][1] = RenderClass::fogRGBA[1];
		JsonSettings["fogRGBA"][2] = RenderClass::fogRGBA[2];

		JsonSettings["doReflections"] = RenderClass::doReflections;
		JsonSettings["doFog"] = RenderClass::doFog;

		JsonSettings["DepthDistance"] = RenderClass::DepthDistance;
		JsonSettings["DepthPlane"][0] = RenderClass::DepthPlane[0];
		JsonSettings["DepthPlane"][1] = RenderClass::DepthPlane[1];

		JsonSettings["Window"] = windowHandler::s_WindowTitle;
		JsonSettings["DefaultSkyboxPath"] = Skybox::DefaultSkyboxPath;

		SettingsData.push_back(JsonSettings);

		// Write to file
		// Write back to file
		std::ofstream outFile(path, std::ios::out);
		if (!outFile.is_open()) {
			if (init::LogALL || init::LogSystems) std::cout << ("Failed to write to ") << path << std::endl;
		}

		outFile << SettingsData.dump(4);
		outFile.close();

		if (init::LogALL || init::LogSystems) std::cout << "Successfully updated " << path << std::endl;

	}
	catch (const std::exception& e) {
		if (init::LogALL || init::LogSystems) std::cout << "Exception: " << e.what() << std::endl;
	}
}

void Scene::JsonCameraSettingsSave(std::string path) {
	try {
		json CameraData = json::array();  // New JSON array to hold model data

		json JsonCamera;
		JsonCamera["initialCameraPos"] = { Scene::initalCameraPos.x, Scene::initalCameraPos.y, Scene::initalCameraPos.z };
		JsonCamera["FOV"] = Main::cameraSettings[0];
		JsonCamera["nearPlane"] = Main::cameraSettings[1];
		JsonCamera["farPlane"] = Main::cameraSettings[2];
		JsonCamera["cameraColliderScale"][0] = Camera::cameraColliderScale.x;
		JsonCamera["cameraColliderScale"][1] = Camera::cameraColliderScale.y;
		JsonCamera["cameraColliderScale"][2] = Camera::cameraColliderScale.z;
		CameraData.push_back(JsonCamera);

		// Write to file
		// Write back to file
		std::ofstream outFile(path, std::ios::out);
		if (!outFile.is_open()) {
			if (init::LogALL || init::LogSystems) std::cout << ("Failed to write to ")<< path << std::endl;
		}

		outFile << CameraData.dump(4);
		outFile.close();

		if (init::LogALL || init::LogSystems) std::cout << "Successfully updated " << path << std::endl;

	}
	catch (const std::exception& e) {
		if (init::LogALL || init::LogSystems) std::cout << "Exception: " << e.what() << std::endl;
	}
}

void Scene::AddSceneModelObject(std::string type, std::string path, std::string name)
{

	std::unique_ptr<ModelObject> newObject = std::make_unique<ModelObject>(); // Use std::make_unique
	newObject->CreateObject(type, path, name, "Assets/Material/Default.Material");
	modelObjects.push_back(std::move(newObject)); // std::move is crucial here
}

void Scene::AddSceneBillBoardObject(std::string name, std::string type, std::string path) {

	BillBoardObject newBillBoardObject; // Create a temporary BillBoardObject
	newBillBoardObject.CreateObject(type, path, name);
	BillBoardObjects.push_back(newBillBoardObject);
}

void Scene::AddSceneSoundObject(std::string name, std::string path) {

}

void Scene::AddSceneColliderObject(std::string name) {

	CubeCollider newCubeColliderObject; // Create a temporary CubeCollider
	newCubeColliderObject.init();
	newCubeColliderObject.name = name;
	CubeColliderObject.push_back(newCubeColliderObject);
}

void Scene::AddSceneLightObject() {
	colour.push_back(glm::vec3(1.0f, 1.0f, 1.0f)); // Default white color
	position.push_back(glm::vec3(0.0f, 0.0f, 0.0f)); // Default position at origin
	radiusAndPower.push_back(glm::vec2(100.0f, 5.0f)); // Default radius and power
	lightType.push_back(1); // Default to Pointlight
	enabled.push_back(1); // Default enabled state
}

void Scene::initJsonBillBoardLoad(std::string path) {
	std::ifstream file(path);
	if (!file.is_open()) {
		std::cout << "Failed to open file: " << path << std::endl;
		return;
	}
	json BillBoardFileData;
	try {
		file >> BillBoardFileData;
	}
	catch (const nlohmann::json::parse_error& e) {
		// This catch block specifically handles JSON parsing errors,
		// which gives more precise error information from the library.
		std::cout << "JSON Parse Error loading BillBoard data: " << e.what() << std::endl;
		std::cout << "Error byte position: " << e.byte << std::endl; // Specific to nlohmann::json
	}
	catch (const std::ios_base::failure& e) {
		// This catch block handles file I/O errors (e.g., file not found, permission issues).
		std::cout << "File I/O Error loading BillBoard data: " << e.what() << std::endl;
	}
	catch (const std::exception& e) {
		// A general catch-all for any other std::exception derived errors.
		std::cout << "An unexpected error occurred loading BillBoard data: " << e.what() << std::endl;
	}
	file.close();

	for (const auto& item : BillBoardFileData) {

		BillBoardObject newBillBoardObject;
		std::string name = item.at("name").get<std::string>();
		std::string type = item.at("type").get<std::string>();
		std::string path = item.at("path").get<std::string>();

		bool doPitch = item.at("doPitch").get<bool>();
		bool isCollider = item.at("isCollider").get<bool>();
		bool DoFrustumCull = item.at("DoFrustumCull").get<bool>();
		bool doUpdateSequence;
		if (item.contains("doUpdateSequence")) {
			doUpdateSequence = item.at("doUpdateSequence").get<bool>();
			newBillBoardObject.doUpdateSequence = doUpdateSequence;
		}
		unsigned int tickrate = 20;
		if (item.contains("tickrate")) {
			tickrate = item.at("tickrate").get<float>();
			newBillBoardObject.tickrate = tickrate;
		}

		glm::vec3 position = glm::vec3(item.at("position")[0], item.at("position")[1], item.at("position")[2]);
		glm::vec3 scale = glm::vec3(item.at("scale")[0], item.at("scale")[1], item.at("scale")[2]);

		newBillBoardObject.CreateObject(type, path, name);
		newBillBoardObject.doPitch = doPitch;
		newBillBoardObject.isCollider = isCollider;
		newBillBoardObject.DoFrustumCull = DoFrustumCull;
		newBillBoardObject.transform = position;
		newBillBoardObject.scale = scale;

		BillBoardObjects.push_back(newBillBoardObject); // Add the configured object to the vector
	}
	if (init::LogALL || init::LogModel) std::cout << "Loaded Scene BillBoards from: " << path << std::endl;
}

void Scene::initJsonColliderLoad(std::string path) {
	std::ifstream file(path);
	if (!file.is_open()) {
		std::cout << "Failed to open file: " << path << std::endl;
		return;
	}
	json CubeColliderFileData;
	try {
		file >> CubeColliderFileData;
	}
	catch (const nlohmann::json::parse_error& e) {
		// This catch block specifically handles JSON parsing errors,
		// which gives more precise error information from the library.
		std::cout << "JSON Parse Error loading CubeCollider data: " << e.what() << std::endl;
		std::cout << "Error byte position: " << e.byte << std::endl; // Specific to nlohmann::json
	}
	catch (const std::ios_base::failure& e) {
		// This catch block handles file I/O errors (e.g., file not found, permission issues).
		std::cout << "File I/O Error loading CubeCollider data: " << e.what() << std::endl;
	}
	catch (const std::exception& e) {
		// A general catch-all for any other std::exception derived errors.
		std::cout << "An unexpected error occurred loading CubeCollider data: " << e.what() << std::endl;
	}
	file.close();

	for (const auto& item : CubeColliderFileData) {

		CubeCollider newCubeCollider;
		std::string name = item.at("name").get<std::string>();
		bool enabled = item.at("enabled").get<bool>();
		glm::vec3 position = glm::vec3(item.at("position")[0], item.at("position")[1], item.at("position")[2]);
		glm::vec3 scale = glm::vec3(item.at("scale")[0], item.at("scale")[1], item.at("scale")[2]);

		newCubeCollider.init();
		newCubeCollider.name = name;
		newCubeCollider.colliderXYZ = position;
		newCubeCollider.colliderScale = scale;
		newCubeCollider.enabled = enabled;

		CubeColliderObject.push_back(newCubeCollider); // Add the configured object to the vector
	}
	if (init::LogALL || init::LogModel) std::cout << "Loaded Scene CubeColliders from: " << path << std::endl;
}

void Scene::initJsonSoundObjectLoad(std::string path) {
	std::ifstream file(path);
	if (!file.is_open()) {
		std::cout << "Failed to open file: " << path << std::endl;
		return;
	}
	json CubeColliderFileData;
	try {
		file >> CubeColliderFileData;
	}
	catch (const nlohmann::json::parse_error& e) {
		// This catch block specifically handles JSON parsing errors,
		// which gives more precise error information from the library.
		std::cout << "JSON Parse Error loading SoundObject data: " << e.what() << std::endl;
		std::cout << "Error byte position: " << e.byte << std::endl; // Specific to nlohmann::json
	}
	catch (const std::ios_base::failure& e) {
		// This catch block handles file I/O errors (e.g., file not found, permission issues).
		std::cout << "File I/O Error loading SoundObject data: " << e.what() << std::endl;
	}
	catch (const std::exception& e) {
		// A general catch-all for any other std::exception derived errors.
		std::cout << "An unexpected error occurred loading SoundObject data: " << e.what() << std::endl;
	}
	file.close();
	for (const auto& item : CubeColliderFileData) {

		SoundProgram newSoundObject;
		std::string path = item.at("path").get<std::string>();
		std::string name = item.at("name").get<std::string>();
		float pitch = item.at("pitch").get<float>();
		float volume = item.at("volume").get<float>();
		bool isLoop = item.at("isLoop").get<bool>();
		glm::vec3 SoundPosition = glm::vec3(item.at("SoundPosition")[0],
			item.at("SoundPosition")[1], item.at("SoundPosition")[2]);
		bool is3DSound = item.at("is3Dsound").get<bool>();

		newSoundObject.CreateSound(path, name);
		newSoundObject.SetPitch(pitch);
		newSoundObject.SetVolume(volume);
		newSoundObject.SetSoundPosition(SoundPosition.x, SoundPosition.y, SoundPosition.z);
		newSoundObject.Set3D(is3DSound); // seems the actual soundclass doesnt like this, so ill move on and fix this later down the line

		isSoundLoop.push_back(isLoop);
		SoundObjects.push_back(newSoundObject); // Add the configured object to the vector
	}
	if (init::LogALL || init::LogModel) std::cout << "Loaded Scene SoundObject from: " << path << std::endl;
}
	
void Scene::initJsonLightObjectLoad(std::string path) {
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
		glm::vec3 newColour = glm::vec3(item.at("colour")[0], item.at("colour")[1], item.at("colour")[2]);
		std::string newType = item.at("type").get<std::string>();
		bool newEnabled = item.at("enabled").get<bool>();

		float newRadius = item.at("radius").get<float>();
		float newPower = item.at("power").get<float>();


		position.push_back(newPosition);
		colour.push_back(newColour);
		
		if (newType == "pointlight" || newType == "Pointlight") {
			lightType.push_back(1); // Point light
		}
		else if (newType == "spotlight" || newType == "Spotlight" ) {
			lightType.push_back(0); // Spotlight
		}
		enabled.push_back(newEnabled);
		radiusAndPower.push_back(glm::vec2(newRadius, newPower));
		
		// pushback here
	}
	if (init::LogALL || init::LogModel) std::cout << "Loaded Scene LightObject from: " << path << std::endl;



}

void Scene::initJsonSettingsLoad(std::string path) {
	std::ifstream engineDefaultFile(path);
	if (engineDefaultFile.is_open()) {
		json engineDefaultData;
		engineDefaultFile >> engineDefaultData;
		engineDefaultFile.close();

		//Camera::Position.x = engineDefaultData[0]["DefaultCameraPos"][0];
		//Camera::Position.x = engineDefaultData[0]["DefaultCameraPos"][1];
		//Camera::Position.x = engineDefaultData[0]["DefaultCameraPos"][2];

		RenderClass::skyRGBA[0] = engineDefaultData[0]["skyRGBA"][0];
		RenderClass::skyRGBA[1] = engineDefaultData[0]["skyRGBA"][1];
		RenderClass::skyRGBA[2] = engineDefaultData[0]["skyRGBA"][2];

		RenderClass::fogRGBA[0] = engineDefaultData[0]["fogRGBA"][0];
		RenderClass::fogRGBA[1] = engineDefaultData[0]["fogRGBA"][1];
		RenderClass::fogRGBA[2] = engineDefaultData[0]["fogRGBA"][2];

		RenderClass::doReflections = engineDefaultData[0]["doReflections"];
		RenderClass::doFog = engineDefaultData[0]["doFog"];

		RenderClass::DepthDistance = engineDefaultData[0]["DepthDistance"];
		RenderClass::DepthPlane[0] = engineDefaultData[0]["DepthPlane"][0];
		RenderClass::DepthPlane[1] = engineDefaultData[0]["DepthPlane"][1];

		windowHandler::s_WindowTitle = engineDefaultData[0]["Window"];
		glfwSetWindowTitle(windowHandler::window, (windowHandler::s_WindowTitle).c_str());
		
		// window name needs to be set here

		Skybox::DefaultSkyboxPath = engineDefaultData[0]["DefaultSkyboxPath"];
		Skybox::LoadSkyBoxTexture(Skybox::DefaultSkyboxPath);
	}
	else {
		std::cerr << "Failed to open " << path << std::endl;
	}
}

void Scene::initCameraSettingsLoad(std::string path) {
	std::ifstream CameraFile(path);
	if (CameraFile.is_open()) {
		json CameraData;
		CameraFile >> CameraData;
		CameraFile.close();

		initalCameraPos = glm::vec3((CameraData[0]["initialCameraPos"][0]),
			(CameraData[0]["initialCameraPos"][1]),
			(CameraData[0]["initialCameraPos"][2])
		);
		Camera::Position = Scene::initalCameraPos;
		float fov = CameraData[0]["FOV"].get<float>();
		//std::cout << fov << std::endl;
		float nearPlane = CameraData[0]["nearPlane"].get<float>();
		float farPlane = CameraData[0]["farPlane"].get<float>();
		Main::cameraSettings[0] = fov;
		Main::cameraSettings[1] = nearPlane;
		Main::cameraSettings[2] = farPlane;
		Camera::cameraColliderScale.x = CameraData[0]["cameraColliderScale"][0];
		Camera::cameraColliderScale.y = CameraData[0]["cameraColliderScale"][1];
		Camera::cameraColliderScale.z = CameraData[0]["cameraColliderScale"][2];

	}
	else {
		std::cerr << "Failed to open " << path << std::endl;
	}
}

void Scene::Update() {

	// models
	for (size_t i = 0; i < modelObjects.size(); i++)
	{
		
			//if (modelObjects[i].ModelSingle.collide(glm::vec3(Camera::Position.x, (Camera::Position.y ), Camera::Position.z), Camera::cameraColliderScale, "box")) {
			//Player::isColliding = true; // Set collision state
			//Camera::Position.x = modelObjects[i].ModelSingle.lastHit.x;//- (Camera::cameraColliderScale.y / 2.0f)
			//Camera::Position.y = ((modelObjects[i].ModelSingle.lastHit.y )); //+ (Camera::cameraColliderScale.y / 2.0f)
			//Camera::Position.z = modelObjects[i].ModelSingle.lastHit.z;

			//}
		modelObjects[i]->UpdateCollider();
		modelObjects[i]->UpdateCameraCollider();


		// before drawing we wanna update the lights in the material class

		modelObjects[i]->updateForwardLights(colour, position, radiusAndPower, lightType, enabled); // needs to take lights

		modelObjects[i]->draw();
	}
	// billboards
	for (size_t i = 0; i < BillBoardObjects.size(); i++)
	{
		BillBoardObjects[i].UpdateCollider();
		BillBoardObjects[i].UpdateCameraCollider();
		BillBoardObjects[i].draw();
	}

	for (size_t i = 0; i < CubeColliderObject.size(); i++) {
		CubeColliderObject[i].update();
		CubeColliderObject[i].draw();

	}

	for (size_t i = 0; i < SoundObjects.size(); i++) {
		//std::cout << (i + 1) << std::endl;
		SoundObjects[i].updateCameraPosition();
		if (!SoundObjects[i].isPlay) {
			SoundObjects[i].PlaySound();
		}
	}
		

	/*
		// range Scene::enabled
		//if (true) {
			RenderClass::shaderProgram.Activate();

			RenderClass::shaderProgram.setFloat3Vector(
				"lightPos2",
				position.size(),
				&position[0].x
			);

			RenderClass::shaderProgram.setFloat3Vector(
				"colour2",
				colour.size(),
				&colour[0].x
			);
			//colour

			RenderClass::shaderProgram.setInt(
				"sizeOfLights",
				enabled.size()
			);

			RenderClass::shaderProgram.setFloat2Vector(
				"radiusAndPower",
				radiusAndPower.size(),
				&radiusAndPower[0].x
			);

			RenderClass::shaderProgram.setIntVector(
				"lightType",
				lightType.size(),
				&lightType[0]
			);
			//radius
			//std::cout << LightObjectList.size() << std::endl;
			//

//			for (size_t i = 0; i < enabled.size(); i++)
//			{
//				if (!VisibilityChecker::isInRange(position[i], Camera::Position, 12)) {
//					if (Camera::isRadiusInFrustum(position[i], 2)) {
//						enabled[i] = 1; // Enable light if within frustum
//					}
//					else {
//						enabled[i] = 0; // Disable light if outside frustum
//					}
//				}
//				else {
//					enabled[i] = 1; // Enable light if within frustum
//				}
//
//			}


			RenderClass::shaderProgram.setIntVector("enabled", enabled.size(), &enabled[0]);
				


			RenderClass::shaderProgram.setInt("lightCount", enabled.size());

		//}
	*/
		




	for (size_t i = 0; i < enabled.size(); i++)
	{

		if (FEImGuiWindow::showViewportIcons) {

				if (lightType[i] == 0) {
					SpotLightIcon.draw(true, position[i].x, position[i].y, position[i].z, 0.3, 0.3, 0.3);
				}
				else if (lightType[i] == 1) {
					PointLightIcon.draw(true, position[i].x, position[i].y, position[i].z, 0.3, 0.3, 0.3);
				}
				else if (lightType[i] == 2) {
					//DirectionalLightIcon.draw(true, position[i].x, position[i].y, position[i].z, 0.3, 0.3, 0.3);
				}
		}

		
		//position
		//LightObjectList[i].colour
	}


}

void Scene::Delete() {

	for (size_t i = 0; i < modelObjects.size(); i++)
	{modelObjects[i]->Delete();}
	modelObjects.clear();

	for (size_t i = 0; i < BillBoardObjects.size(); i++)
	{ BillBoardObjects[i].Delete(); }
	BillBoardObjects.clear();

	CubeColliderObject.clear();

	for (size_t i = 0; i < isSoundLoop.size(); i++) {
		if (isSoundLoop[i]) {
			SoundObjects[i].StopSound();
		}
	}

	isSoundLoop.clear();

	for (size_t i = 0; i < SoundObjects.size(); i++)
	{SoundObjects[i].DeleteSound();}
	SoundObjects.clear();

	colour.clear();
	position.clear();
	radiusAndPower.clear();
	lightType.clear();
	enabled.clear();

	if (FEImGuiWindow::imGuiEnabled) {
		FEImGuiWindow::ContentObjects.clear();
		FEImGuiWindow::ContentObjectNames.clear();
		FEImGuiWindow::ContentObjectPaths.clear();
		FEImGuiWindow::ContentObjectTypes.clear();

	}
}