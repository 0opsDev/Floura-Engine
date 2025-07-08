#include "scene.h"

//BillBoardObject BillBoardObjects;
//CubeCollider ColliderObject;
//SoundProgram SoundObjects;
//std::vector<ModelObject> Scene::objects;

std::vector<ModelObject> Scene::modelObjects;
std::vector<BillBoardObject> Scene::BillBoardObjects;
std::vector <CubeCollider> Scene::CubeColliderObject;
std::vector <SoundProgram> Scene::SoundObjects;
std::vector <bool> Scene::isSoundLoop;

void Scene::LoadScene(std::string path) {

	// Attemp to delete previous scene
	Delete();

	initJsonModelLoad(path + "/Model.scene");
	initJsonBillBoardLoad(path + "/BillBoard.scene");
	initJsonColliderLoad(path + "/Collider.scene");
	initJsonSoundObjectLoad(path + "/Sound.scene");
	initJsonSettingsLoad(path + "/Settings.scene");
}

void Scene::SaveScene(std::string path) {
	JsonModelSave(path + "/Model.scene");
	JsonBillBoardSave(path + "/BillBoard.scene");
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
		ModelObject newObject; // Create a temporary ModelObject

		std::string name = item.at("name").get<std::string>();
		std::string type = item.at("type").get<std::string>();
		std::string path = item.at("path").get<std::string>();

		glm::vec3 Location = glm::vec3(item.at("Location")[0], item.at("Location")[1], item.at("Location")[2]);
		glm::vec4 rotation = glm::vec4(item.at("Rotation")[0], item.at("Rotation")[1], item.at("Rotation")[2], item.at("Rotation")[3]);
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

		newObject.CreateObject(type, path, name);

		newObject.transform = Location;
		newObject.rotation = rotation;
		newObject.scale = scale;
		newObject.isCollider = isCollider;
		newObject.DoCulling = isBackFaceCulling;
		newObject.DoFrustumCull = DoFrustumCull;
		newObject.BoxColliderTransform = BoxColliderTransform;
		newObject.BoxColliderScale = BoxColliderScale;
		newObject.frustumBoxTransform = frustumBoxTransform;
		newObject.frustumBoxScale = frustumBoxScale;
		modelObjects.push_back(newObject); // Add the configured object to the vector

	}
	if (init::LogALL || init::LogModel) std::cout << "Loaded Scene Models from: " << path << std::endl;
}

void Scene::JsonModelSave(std::string path) {
	try {
		json settingsData = json::array();  // New JSON array to hold model data

		// Serialize each modelObject into JSON
		for (const auto& obj : modelObjects) {
			json modelJson;
			modelJson["name"] = obj.ObjectName;

			std::string isLOD;
			if (obj.IsLod) isLOD = "LOD";
			else isLOD = "Static";
			modelJson["type"] = isLOD;

			modelJson["path"] = obj.ModelPath;

			modelJson["Location"] = { obj.transform.x, obj.transform.y, obj.transform.z };
			modelJson["Rotation"] = { obj.rotation.x, obj.rotation.y, obj.rotation.z, obj.rotation.w };
			modelJson["Scale"] = { obj.scale.x, obj.scale.y, obj.scale.z };

			modelJson["frustumBoxTransform"] = { obj.frustumBoxTransform.x, obj.frustumBoxTransform.y, obj.frustumBoxTransform.z };
			modelJson["frustumBoxScale"] = { obj.frustumBoxScale.x, obj.frustumBoxScale.y, obj.frustumBoxScale.z };

			modelJson["BoxColliderTransform"] = { obj.BoxColliderTransform.x, obj.BoxColliderTransform.y, obj.BoxColliderTransform.z };
			modelJson["BoxColliderScale"] = { obj.BoxColliderScale.x, obj.BoxColliderScale.y, obj.BoxColliderScale.z };

			modelJson["isCollider"] = obj.isCollider;
			modelJson["isBackFaceCulling"] = obj.DoCulling;
			modelJson["DoFrustumCull"] = obj.DoFrustumCull;

			settingsData.push_back(modelJson);
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

void Scene::AddSceneModelObject(std::string type, std::string path, std::string name)
{
	std::string newName = name;
	for (size_t i = 0; i < modelObjects.size(); i++)
	{
		int numDupes = 0;
		if (newName == modelObjects[i].ObjectName) numDupes++;

		if (numDupes > 0) newName = (newName + " Duplicate");
	}

	ModelObject newObject; // Create a temporary ModelObject
	newObject.CreateObject(type, path, newName);
	modelObjects.push_back(newObject); // Add the configured object to the vector
}

void Scene::AddSceneBillBoardObject(std::string name, std::string type, std::string path) {
	std::string newName = name;
	for (size_t i = 0; i < BillBoardObjects.size(); i++)
	{
		int numDupes = 0;
		if (newName == BillBoardObjects[i].ObjectName) numDupes++;

		if (numDupes > 0) newName = (newName + " Duplicate");
	}
	BillBoardObject newBillBoardObject; // Create a temporary BillBoardObject
	newBillBoardObject.CreateObject(type, path, newName);
	BillBoardObjects.push_back(newBillBoardObject);
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
		bool CollideWithCamera = item.at("CollideWithCamera").get<bool>();
		glm::vec3 position = glm::vec3(item.at("position")[0], item.at("position")[1], item.at("position")[2]);
		glm::vec3 scale = glm::vec3(item.at("scale")[0], item.at("scale")[1], item.at("scale")[2]);

		newCubeCollider.init();
		newCubeCollider.colliderXYZ = position;
		newCubeCollider.colliderScale = scale;
		newCubeCollider.CollideWithCamera = CollideWithCamera;

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
	
void Scene::initJsonSettingsLoad(std::string path) {
	std::ifstream engineDefaultFile(path);
	if (engineDefaultFile.is_open()) {
		json engineDefaultData;
		engineDefaultFile >> engineDefaultData;
		engineDefaultFile.close();

		Main::initalCameraPos = glm::vec3(static_cast<GLfloat>(engineDefaultData[0]["DefaultCameraPos"][0]),
			static_cast<GLfloat>(engineDefaultData[0]["DefaultCameraPos"][1]),
			static_cast<GLfloat>(engineDefaultData[0]["DefaultCameraPos"][2])
		);
		//Camera::Position.x = engineDefaultData[0]["DefaultCameraPos"][0];
		//Camera::Position.x = engineDefaultData[0]["DefaultCameraPos"][1];
		//Camera::Position.x = engineDefaultData[0]["DefaultCameraPos"][2];

		RenderClass::skyRGBA[0] = engineDefaultData[0]["skyRGBA"][0];
		RenderClass::skyRGBA[1] = engineDefaultData[0]["skyRGBA"][1];
		RenderClass::skyRGBA[2] = engineDefaultData[0]["skyRGBA"][2];

		RenderClass::lightRGBA[0] = engineDefaultData[0]["lightRGBA"][0];
		RenderClass::lightRGBA[1] = engineDefaultData[0]["lightRGBA"][1];
		RenderClass::lightRGBA[2] = engineDefaultData[0]["lightRGBA"][2];

		RenderClass::fogRGBA[0] = engineDefaultData[0]["fogRGBA"][0];
		RenderClass::fogRGBA[1] = engineDefaultData[0]["fogRGBA"][1];
		RenderClass::fogRGBA[2] = engineDefaultData[0]["fogRGBA"][2];

		RenderClass::ConeRot[0] = engineDefaultData[0]["ConeRot"][0];
		RenderClass::ConeRot[1] = engineDefaultData[0]["ConeRot"][1];
		RenderClass::ConeRot[2] = engineDefaultData[0]["ConeRot"][2];

		RenderClass::doReflections = engineDefaultData[0]["doReflections"];
		RenderClass::doFog = engineDefaultData[0]["doFog"];
		Main::VertNum = engineDefaultData[0]["VertNum"];
		Main::FragNum = engineDefaultData[0]["FragNum"];

		Main::cameraSettings[1] = std::stof(engineDefaultData[0]["NearPlane"].get<std::string>());
		Main::cameraSettings[2] = std::stof(engineDefaultData[0]["FarPlane"].get<std::string>());

		RenderClass::DepthDistance = engineDefaultData[0]["DepthDistance"];
		RenderClass::DepthPlane[0] = engineDefaultData[0]["DepthPlane"][0];
		RenderClass::DepthPlane[1] = engineDefaultData[0]["DepthPlane"][1];
		SettingsUtils::s_WindowTitle = engineDefaultData[0]["Window"];
		// window name needs to be set here

		Skybox::DefaultSkyboxPath = engineDefaultData[0]["DefaultSkyboxPath"];
	}
	else {
		std::cerr << "Failed to open Settings/Default/EngineDefault.json" << std::endl;
	}
}

void Scene::Update() {

	// models
	for (size_t i = 0; i < modelObjects.size(); i++)
	{
		modelObjects[i].UpdateCollider();
		modelObjects[i].UpdateCameraCollider();
		modelObjects[i].draw(RenderClass::shaderProgram);
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
}

void Scene::Delete() {

	for (size_t i = 0; i < modelObjects.size(); i++)
	{modelObjects[i].Delete();}
	modelObjects.clear();

	for (size_t i = 0; i < BillBoardObjects.size(); i++)
	{ BillBoardObjects[i].Delete(); }
	BillBoardObjects.clear();

	CubeColliderObject.clear();

	for (size_t i = 0; i < SoundObjects.size(); i++)
	{SoundObjects[i].DeleteSound();}
	SoundObjects.clear();

	isSoundLoop.clear();
}