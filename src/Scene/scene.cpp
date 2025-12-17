#include "scene.h"
#include <Render/window/WindowHandler.h>
#include <Gameplay/Player.h>
#include <Scene/LightingHandler.h>
#include <utils/logConsole.h>


std::string Scene::sceneName = ""; // Map loading
std::vector <CubeCollider> Scene::CubeColliderObject;
std::vector <SoundProgram> Scene::SoundObjects;
std::vector <std::unique_ptr<entity>> Scene::entityObjects;

glm::vec3 Scene::initalCameraPos = glm::vec3(0, 0, 0);


BillBoard* PointLightIcon;
BillBoard* SpotLightIcon;
BillBoard* SoundIcon;

void Scene::init() {
	PointLightIcon = new BillBoard("Assets/Icons/point.png");
	SpotLightIcon = new BillBoard("Assets/Icons/spot.png"); // draw instanced option with array argument for transformations
	SoundIcon = new BillBoard("assets/Icons/soundIcon.png");

}

void Scene::LoadScene(std::string path) {

	// Attemp to delete previous scene
	Delete();
	IdManager::onSceneLoad();

	if (FEImGuiWindow::imGuiEnabled) {
		FEImGuiWindow::loadContentObjects(path + "/ContentObject.scene");
	}

	initJsonSettingsLoad(path + "/Settings.scene");
	initJsonColliderLoad(path + "/Collider.scene");
	LightingHandler::loadScene(path + "/Lights.scene");
	JsonEnviromentLoad(path + "/Enviroment.scene"); // gives DefaultSkyboxPath
	Skybox::LoadSkyBoxTexture(Skybox::DefaultSkyboxPath); // cleanup this class, could add a load cubemap texture function to the texture class
	initJsonBillBoardLoad(path + "/BillBoard.scene"); // here
	initJsonModelLoad(path + "/Model.scene");
	initJsonSoundObjectLoad(path + "/Sound.scene");
	initCameraSettingsLoad(path + "/Camera.scene");

	LogConsole::print("Loaded scene from: " + path);
}


void Scene::SaveScene(std::string path) {

	if (FEImGuiWindow::imGuiEnabled) {
		FEImGuiWindow::saveContentObjects(path + "/ContentObject.scene");
	}

	JsonSettingsSave(path + "/Settings.scene");
	JsonColliderSave(path + "/Collider.scene");
	LightingHandler::saveScene(path + "/Lights.scene");
	JsonEnviromentSave(path + "/Enviroment.scene");
	JsonBillBoardSave(path + "/BillBoard.scene");
	JsonModelSave(path + "/Model.scene");
	JsonSoundObjectSave(path + "/Sound.scene");
	JsonCameraSettingsSave(path + "/Camera.scene");

	LogConsole::print("Saved scene to: " + path);
}


void Scene::JsonEnviromentSave(std::string path)
{
	try {
		json EnviromentData = json::array();  // New JSON array to hold model data

		json JsonEnviroment;

		// sky
		JsonEnviroment["skyRGBA"][0] = RenderClass::skyRGBA[0];
		JsonEnviroment["skyRGBA"][1] = RenderClass::skyRGBA[1];
		JsonEnviroment["skyRGBA"][2] = RenderClass::skyRGBA[2];
		JsonEnviroment["DefaultSkyboxPath"] = Skybox::DefaultSkyboxPath;
		JsonEnviroment["DoSkyColour"] = Skybox::DoSbRGBA;
		JsonEnviroment["RenderSkybox"] = RenderClass::renderSkybox;
		JsonEnviroment["SkyRotation"][0] = Skybox::rotation.x;
		JsonEnviroment["SkyRotation"][1] = Skybox::rotation.y;
		JsonEnviroment["SkyRotation"][2] = Skybox::rotation.z;

		// directional light
		JsonEnviroment["DirEnabled"] = LightingHandler::doDirLight;
		JsonEnviroment["DirSpecEnabled"] = LightingHandler::doDirSpecularLight;
		JsonEnviroment["DirRotation"][0] = LightingHandler::dirLightRot[0];
		JsonEnviroment["DirRotation"][1] = LightingHandler::dirLightRot[1];
		JsonEnviroment["DirRotation"][2] = LightingHandler::dirLightRot[2];
		JsonEnviroment["DirAmbient"] = LightingHandler::directAmbient;
		JsonEnviroment["DirSpecular"] = LightingHandler::dirSpecularLight;
		JsonEnviroment["DirColour"][0] = LightingHandler::directLightCol[0];
		JsonEnviroment["DirColour"][1] = LightingHandler::directLightCol[1];
		JsonEnviroment["DirColour"][2] = LightingHandler::directLightCol[2];
		JsonEnviroment["DoShadowMap"] = LightingHandler::doDirShadowMap;
		JsonEnviroment["dirNearFar"][0] = LightingHandler::dirNearFar[0];
		JsonEnviroment["dirNearFar"][1] = LightingHandler::dirNearFar[1];
		JsonEnviroment["dirSmDistance"] = LightingHandler::distance;
		JsonEnviroment["dirSmHeight"] = LightingHandler::dirShadowheight;
		JsonEnviroment["dirShadowMapHardness"] = LightingHandler::dirShadowMapHardness;
		JsonEnviroment["dirShadowMapSamples"] = LightingHandler::dirShadowMapSamples;
		JsonEnviroment["DirSMMaxBias"] = LightingHandler::DirSMMaxBias;
		/*
		DirEnabled
		DirSpecEnabled
		DirRotation
		DirAmbient
		DirSpecular
		DirColour
		*/

		/*
		JsonEnviroment["fogRGBA"][0] = RenderClass::fogRGBA[0];
		JsonEnviroment["fogRGBA"][1] = RenderClass::fogRGBA[1];
		JsonEnviroment["fogRGBA"][2] = RenderClass::fogRGBA[2];
		*/

		/*
			JsonEnviroment["doReflections"] = RenderClass::doReflections;
			JsonEnviroment["doFog"] = RenderClass::doFog; 
		 */

		EnviromentData.push_back(JsonEnviroment);

		// Write to file
		// Write back to file
		std::ofstream outFile(path, std::ios::out);
		if (!outFile.is_open()) {

			std::cout << ("Failed to write to ") << path << std::endl;
		}

		outFile << EnviromentData.dump(4);
		outFile.close();

		std::cout << "Successfully updated " << path << std::endl;

	}
	catch (const std::exception& e) {
		std::cout << "Exception: " << e.what() << std::endl;
	}
}

void Scene::JsonEnviromentLoad(std::string path)
{
	std::ifstream EnviromentDefaultFile(path);
	if (EnviromentDefaultFile.is_open()) {
		json EnviromentDefaultData;
		EnviromentDefaultFile >> EnviromentDefaultData;
		EnviromentDefaultFile.close();

		// sky
		RenderClass::skyRGBA[0] = EnviromentDefaultData[0]["skyRGBA"][0];
		RenderClass::skyRGBA[1] = EnviromentDefaultData[0]["skyRGBA"][1];
		RenderClass::skyRGBA[2] = EnviromentDefaultData[0]["skyRGBA"][2];
		Skybox::DefaultSkyboxPath = EnviromentDefaultData[0]["DefaultSkyboxPath"];
		Skybox::LoadSkyBoxTexture(Skybox::DefaultSkyboxPath);
		Skybox::DoSbRGBA = EnviromentDefaultData[0]["DoSkyColour"];;
		RenderClass::renderSkybox = EnviromentDefaultData[0]["RenderSkybox"];;
		Skybox::rotation.x = EnviromentDefaultData[0]["SkyRotation"][0];
		Skybox::rotation.y = EnviromentDefaultData[0]["SkyRotation"][1];
		Skybox::rotation.z = EnviromentDefaultData[0]["SkyRotation"][2];

		// directional light
		LightingHandler::doDirLight = EnviromentDefaultData[0]["DirEnabled"];
		LightingHandler::doDirSpecularLight = EnviromentDefaultData[0]["DirSpecEnabled"];
		LightingHandler::dirLightRot[0] = EnviromentDefaultData[0]["DirRotation"][0];
		LightingHandler::dirLightRot[1] = EnviromentDefaultData[0]["DirRotation"][1];
		LightingHandler::dirLightRot[2] = EnviromentDefaultData[0]["DirRotation"][2];
		LightingHandler::directAmbient = EnviromentDefaultData[0]["DirAmbient"];
		LightingHandler::dirSpecularLight = EnviromentDefaultData[0]["DirSpecular"];
		LightingHandler::directLightCol[0] = EnviromentDefaultData[0]["DirColour"][0];
		LightingHandler::directLightCol[1] = EnviromentDefaultData[0]["DirColour"][1];
		LightingHandler::directLightCol[2] = EnviromentDefaultData[0]["DirColour"][2];
		LightingHandler::doDirShadowMap = EnviromentDefaultData[0]["DoShadowMap"];
		LightingHandler::dirNearFar[0] = EnviromentDefaultData[0]["dirNearFar"][0];
		LightingHandler::dirNearFar[1] = EnviromentDefaultData[0]["dirNearFar"][1];
		LightingHandler::distance = EnviromentDefaultData[0]["dirSmDistance"];
		LightingHandler::dirShadowheight = EnviromentDefaultData[0]["dirSmHeight"];
		LightingHandler::dirShadowMapHardness = EnviromentDefaultData[0]["dirShadowMapHardness"];
		LightingHandler::dirShadowMapSamples = EnviromentDefaultData[0]["dirShadowMapSamples"];
		LightingHandler::DirSMMaxBias = EnviromentDefaultData[0]["DirSMMaxBias"];
	}
	else {
		std::cerr << "Enviroment Failed to open " << path << std::endl;
	}
}

void Scene::initJsonModelLoad(std::string path) {
	std::ifstream file(path);
	if (!file.is_open()) {
		std::cout << "Model Failed to open file: " << path << std::endl;
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
		std::unique_ptr<entity> newObject = std::make_unique<entity>(); // Use std::make_unique

		std::string name = item.at("name").get<std::string>();
		std::string path = item.at("path").get<std::string>();
		std::string MaterialPath = item.at("MaterialPath").get<std::string>();

		// transform
		newObject->setPosition(glm::vec3(item.at("Location")[0], item.at("Location")[1], item.at("Location")[2]));
		newObject->setRotation(glm::vec3(item.at("Rotation")[0], item.at("Rotation")[1], item.at("Rotation")[2]));
		newObject->setScale(glm::vec3(item.at("Scale")[0], item.at("Scale")[1], item.at("Scale")[2]));

		newObject->setDoCulling(item.at("isBackFaceCulling").get<bool>());
		newObject->SetCastsShadow(item.at("CastShadow").get<bool>());
		newObject->setUVScale(glm::vec2(item.at("uvScale")[0],
			item.at("uvScale")[1]));

		newObject->ID.UniqueNumber = item.at("IDuniqueIdentifier").get<unsigned int>();
		newObject->component.renderHeads.smoothnessValue = item.at("smoothnessValue").get<float>();

		//newObject->create('m', name, path, MaterialPath); // Load into this unique MaterialObject // this needs to run and somehow join up when complete?
		// what about the idea of creating them in a state without a actual model, then doing the create function on a thread and push back when joinable;
		
		// doRender
		newObject->component.flags.render = item.at("doRender").get<bool>();

		entityObjects.push_back(std::move(newObject));
		
		entityObjects.back()->create('m', name, path, MaterialPath);



	}
	std::cout << "Loaded Scene Models from: " << path << std::endl;
}

void Scene::JsonModelSave(std::string path) {
	try {
		json settingsData = json::array();  // New JSON array to hold model data
		for (size_t i = 0; i < entityObjects.size(); i++)
		{
			if (entityObjects[i]->fetchType() == 'm') // model
			{
				json modelJson;
				modelJson["name"] = entityObjects[i]->fetchName();

				modelJson["path"] = entityObjects[i]->fetchPath();
				glm::vec3 objPos = entityObjects[i]->fetchPosition();
				glm::vec3 objScale = entityObjects[i]->fetchScale();
				glm::vec3 objRot = entityObjects[i]->fetchRotation();

				modelJson["Location"] = { objPos.x, objPos.y, objPos.z };
				modelJson["Rotation"] = { objRot.x, objRot.y, objRot.z };
				modelJson["Scale"] = { objScale.x, objScale.y, objScale.z };

				modelJson["isBackFaceCulling"] = entityObjects[i]->fetchDoCulling();
				modelJson["MaterialPath"] = entityObjects[i]->component.systems.material.Material.materialPath;
				modelJson["CastShadow"] = entityObjects[i]->FetchCastsShadow();
				glm::vec2 uvScale = entityObjects[i]->fetchUVScale();
				modelJson["uvScale"] = { uvScale.x, uvScale.y };
				// ID
				modelJson["IDuniqueIdentifier"] = entityObjects[i]->ID.UniqueNumber;
				//doRender
				modelJson["doRender"] = entityObjects[i]->component.flags.render;
				modelJson["smoothnessValue"] = entityObjects[i]->component.renderHeads.smoothnessValue;

				settingsData.push_back(modelJson);
			}

		}
		// Serialize each modelObject into JSON
		

		// Write to file
		std::ofstream outFile(path, std::ios::out);
		if (!outFile.is_open()) {
			std::cout << "Failed to write to " << path << std::endl;
			return;
		}

		outFile << settingsData.dump(4);  // Pretty-print with indentation
		outFile.close();

		std::cout << "Successfully updated " << path << std::endl;

	}
	catch (const std::exception& e) {
		std::cout << "Exception: " << e.what() << std::endl;
	}
}

void Scene::JsonBillBoardSave(std::string path) {
	try {
		json settingsData = json::array();  // New JSON array to hold model data

		// Serialize each modelObject into JSON
		for (size_t i = 0; i < entityObjects.size(); i++)
		{
			if (entityObjects[i]->fetchType() == 'b')
			{
				json BillBoardJson;
				BillBoardJson["name"] = entityObjects[i]->fetchName();
				BillBoardJson["path"] = entityObjects[i]->fetchPath();

				BillBoardJson["doPitch"] = entityObjects[i]->FetchDoPitch();

				glm::vec3 objPos = entityObjects[i]->fetchPosition();
				glm::vec3 objScale = entityObjects[i]->fetchScale();

				BillBoardJson["position"] = { objPos.x, objPos.y, objPos.z };
				BillBoardJson["scale"] = { objScale.x, objScale.y, objScale.z };

				// ID
				BillBoardJson["IDuniqueIdentifier"] = entityObjects[i]->ID.UniqueNumber;

				settingsData.push_back(BillBoardJson);
			}
		}
		// Write to file
		std::ofstream outFile(path, std::ios::out);
		if (!outFile.is_open()) {
			std::cout << "Failed to write to " << path << std::endl;
			return;
		}

		outFile << settingsData.dump(4);  // Pretty-print with indentation
		outFile.close();

		std::cout << "Successfully updated " << path << std::endl;

	}
	catch (const std::exception& e) {
		std::cout << "Exception: " << e.what() << std::endl;
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
			// ID
			CubeColliderJson["IDuniqueIdentifier"] = obj.ID.UniqueNumber;


			ColliderData.push_back(CubeColliderJson);
		}

		// Write to file
		std::ofstream outFile(path, std::ios::out);
		if (!outFile.is_open()) {
			std::cout << "Failed to write to " << path << std::endl;
			return;
		}

		outFile << ColliderData.dump(4);  // Pretty-print with indentation
		outFile.close();

		std::cout << "Successfully updated " << path << std::endl;

	}
	catch (const std::exception& e) {
		std::cout << "Exception: " << e.what() << std::endl;
	}
}
void Scene::JsonSettingsSave(std::string path) {
	try {
		json SettingsData = json::array();  // New JSON array to hold model data

		json JsonSettings;

		//JsonSettings["skyRGBA"][0] = RenderClass::skyRGBA[0];
		//JsonSettings["skyRGBA"][1] = RenderClass::skyRGBA[1];
		//JsonSettings["skyRGBA"][2] = RenderClass::skyRGBA[2];

		JsonSettings["fogRGBA"][0] = RenderClass::fogRGBA[0];
		JsonSettings["fogRGBA"][1] = RenderClass::fogRGBA[1];
		JsonSettings["fogRGBA"][2] = RenderClass::fogRGBA[2];

		JsonSettings["doReflections"] = RenderClass::doReflections;
		JsonSettings["doFog"] = RenderClass::doFog;

		JsonSettings["DepthDistance"] = RenderClass::DepthDistance;
		JsonSettings["DepthPlane"][0] = RenderClass::DepthPlane[0];
		JsonSettings["DepthPlane"][1] = RenderClass::DepthPlane[1];

		JsonSettings["Window"] = windowHandler::s_WindowTitle;
		//JsonSettings["DefaultSkyboxPath"] = Skybox::DefaultSkyboxPath;

		SettingsData.push_back(JsonSettings);

		// Write to file
		// Write back to file
		std::ofstream outFile(path, std::ios::out);
		if (!outFile.is_open()) {
			std::cout << ("Failed to write to ") << path << std::endl;
		}

		outFile << SettingsData.dump(4);
		outFile.close();

		std::cout << "Successfully updated " << path << std::endl;

	}
	catch (const std::exception& e) {
		std::cout << "Exception: " << e.what() << std::endl;
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
		JsonCamera["cameraColliderScale"][0] = Player::cameraColliderScale.x;
		JsonCamera["cameraColliderScale"][1] = Player::cameraColliderScale.y;
		JsonCamera["cameraColliderScale"][2] = Player::cameraColliderScale.z;
		JsonCamera["gamma"] = Camera::gamma;

		CameraData.push_back(JsonCamera);

		// Write to file
		// Write back to file
		std::ofstream outFile(path, std::ios::out);
		if (!outFile.is_open()) {
			std::cout << ("Failed to write to ")<< path << std::endl;
		}

		outFile << CameraData.dump(4);
		outFile.close();

		std::cout << "Successfully updated " << path << std::endl;

	}
	catch (const std::exception& e) {
		std::cout << "Exception: " << e.what() << std::endl;
	}
}

void Scene::AddSceneSoundObject(std::string name, std::string path) {
	LogConsole::print("not implemented");
	SoundProgram nSoundProjram;
	nSoundProjram.ChangeSound(path);
	nSoundProjram.name = name;
	nSoundProjram.Set3D(true);
	SoundObjects.push_back(nSoundProjram);
}

void Scene::AddSceneColliderObject(std::string name) {

	CubeCollider newCubeColliderObject; // Create a temporary CubeCollider
	newCubeColliderObject.init();
	newCubeColliderObject.name = name;
	CubeColliderObject.push_back(newCubeColliderObject);
	LogConsole::print("Created CubeCollider Object: " + name);
}


void Scene::AddEntityObject(char type, std::string name, std::string path)
{
	std::unique_ptr<entity> newEntity = std::make_unique<entity>(); // Use std::make_unique
	newEntity->create(type, name, path, "Assets/Material/Default.Material");
	entityObjects.emplace_back(std::move(newEntity));
	LogConsole::print("Created Entity: " + name);
}

void Scene::initJsonBillBoardLoad(std::string path) {
	std::ifstream file(path);
	if (!file.is_open()) {
		std::cout << "billboard Failed to open file: " << path << std::endl;
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
		std::unique_ptr<entity> newEntity = std::make_unique<entity>(); // Use std::make_unique

		//std::cout << "BillBoard doPitch: " << item.at("doPitch") <<std::endl;
		newEntity->setPosition(glm::vec3(item.at("position")[0], item.at("position")[1], item.at("position")[2]));
		newEntity->setScale(glm::vec3(item.at("scale")[0], item.at("scale")[1], item.at("scale")[2]));
		// IDs
		newEntity->ID.UniqueNumber = item.at("IDuniqueIdentifier").get<unsigned int>();

		std::string name = item.at("name").get<std::string>();
		std::string nPath = item.at("path").get<std::string>();
		newEntity->create('b', name, nPath, ""); // type, name, path, materialpath // add material path for bb later
		
		newEntity->setDoPitch(item.at("doPitch"));
		entityObjects.push_back(std::move(newEntity)); // Add the configured object to the vector
	}
	std::cout << "Loaded Scene BillBoards from: " << path << std::endl;
}

void Scene::initJsonColliderLoad(std::string path) {
	std::ifstream file(path);
	if (!file.is_open()) {
		std::cout << "Collider Failed to open file: " << path << std::endl;
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
		// ID
		newCubeCollider.ID.UniqueNumber = item.at("IDuniqueIdentifier").get<unsigned int>();

		CubeColliderObject.push_back(newCubeCollider); // Add the configured object to the vector
	}
	std::cout << "Loaded Scene CubeColliders from: " << path << std::endl;
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
		newSoundObject.SetSoundPosition(SoundPosition);
		newSoundObject.Set3D(is3DSound); // seems the actual soundclass doesnt like this, so ill move on and fix this later down the line
		newSoundObject.loop = isLoop;
		SoundObjects.push_back(newSoundObject); // Add the configured object to the vector
	}
	std::cout << "Loaded Scene SoundObject from: " << path << std::endl;
}

void Scene::JsonSoundObjectSave(std::string path)
{
	try {
		json SoundData = json::array();  // New JSON array to hold model data

		// Serialize each modelObject into JSON
		for (const auto& obj : SoundObjects) {
			json SoundJson;
			SoundJson["name"] = obj.name;
			SoundJson["path"] = obj.path;
			SoundJson["pitch"] = obj.pitch;
			SoundJson["volume"] = obj.currentvolume;
			SoundJson["isLoop"] = obj.loop;
			SoundJson["SoundPosition"] = { obj.position.x, obj.position.y, obj.position.z };
			SoundJson["is3Dsound"] = obj.is3D;

			SoundData.push_back(SoundJson);
		}

		// Write to file
		std::ofstream outFile(path, std::ios::out);
		if (!outFile.is_open()) {
			std::cout << "Failed to write to " << path << std::endl;
			return;
		}

		outFile << SoundData.dump(4);  // Pretty-print with indentation
		outFile.close();

		std::cout << "Successfully updated " << path << std::endl;

	}
	catch (const std::exception& e) {
		std::cout << "Exception: " << e.what() << std::endl;
	}
}

void Scene::initJsonSettingsLoad(std::string path) {
	std::ifstream engineDefaultFile(path);
	if (engineDefaultFile.is_open()) {
		json engineDefaultData;
		engineDefaultFile >> engineDefaultData;
		engineDefaultFile.close();

		//RenderClass::skyRGBA[0] = engineDefaultData[0]["skyRGBA"][0];
		//RenderClass::skyRGBA[1] = engineDefaultData[0]["skyRGBA"][1];
		//RenderClass::skyRGBA[2] = engineDefaultData[0]["skyRGBA"][2];

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

		//Skybox::DefaultSkyboxPath = engineDefaultData[0]["DefaultSkyboxPath"];
		//Skybox::LoadSkyBoxTexture(Skybox::DefaultSkyboxPath);
	}
	else {
		std::cerr << "Settings Failed to open " << path << std::endl;
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
		Player::cameraColliderScale.x = CameraData[0]["cameraColliderScale"][0];
		Player::cameraColliderScale.y = CameraData[0]["cameraColliderScale"][1];
		Player::cameraColliderScale.z = CameraData[0]["cameraColliderScale"][2];
		Camera::gamma = CameraData[0]["gamma"].get<float>();

	}
	else {
		std::cerr << "Camera Failed to open " << path << std::endl;
	}
}


void Scene::shadowmapDraw()
{
	for (size_t i = 0; i < entityObjects.size(); i++)
	{
		entityObjects[i]->drawShadowMap();
	}
}

void Scene::draw() 
{
	// entities shadow map should go above here
	for (size_t i = 0; i < entityObjects.size(); i++)
	{
		entityObjects[i]->updateLights();
		entityObjects[i]->draw();
	}

	for (size_t i = 0; i < CubeColliderObject.size(); i++) {
		CubeColliderObject[i].draw();
	}
	if (FEImGuiWindow::showViewportIcons) {
		for (size_t i = 0; i < SoundObjects.size(); i++) {
			if (SoundObjects[i].is3D)
			{
				SoundIcon->doPitch = true;
				SoundIcon->updatePosition(SoundObjects[i].position);
				SoundIcon->updateScale(glm::vec3(1));
				SoundIcon->draw();
			}
		}
		for (size_t i = 0; i < LightingHandler::Lights.size(); i++)
		{

			if (LightingHandler::Lights[i].type == 0) {
				SpotLightIcon->doPitch = true;
				SpotLightIcon->updatePosition(LightingHandler::Lights[i].position);
				SpotLightIcon->updateScale(glm::vec3(0.3f));
				SpotLightIcon->draw();
			}
			else if (LightingHandler::Lights[i].type) {
				PointLightIcon->doPitch = true;
				PointLightIcon->updatePosition(LightingHandler::Lights[i].position);
				PointLightIcon->updateScale(glm::vec3(0.3f));
				PointLightIcon->draw();
			}
		}
	}
}


void Scene::Update() {

	for (size_t i = 0; i < entityObjects.size(); i++)
	{
		entityObjects[i]->update();
	}

	for (size_t i = 0; i < CubeColliderObject.size(); i++) {
		CubeColliderObject[i].update();
	}

	for (size_t i = 0; i < SoundObjects.size(); i++) {

		if (SoundObjects[i].is3D)
		{
			SoundObjects[i].updateCameraPosition();
			SoundObjects[i].SetSoundPosition(SoundObjects[i].position);
		}
		
		if (!SoundObjects[i].isPlay && SoundObjects[i].loop || !SoundObjects[i].isPlay && SoundObjects[i].queuedPlay) {
			SoundObjects[i].PlaySound();
		}
	}


}

void Scene::Delete() {
	// deselect object probably
	FEImGuiWindow::SelectedObjectIndex = -1;
	FEImGuiWindow::SelectedObjectType = ' ';
	
	for (size_t i = 0; i < entityObjects.size(); i++)
	{
		entityObjects[i]->Delete(); }
	entityObjects.clear();

	CubeColliderObject.clear();

	for (size_t i = 0; i < SoundObjects.size(); i++) {
		if (SoundObjects[i].isPlay) {
			SoundObjects[i].StopSound();
		}
	}

	SoundObjects.clear();

	for (size_t i = 0; i < SoundObjects.size(); i++)
	{SoundObjects[i].DeleteSound();}
	SoundObjects.clear();

	LightingHandler::deleteScene();
	
	// no skybox delete for now

	if (FEImGuiWindow::imGuiEnabled) {
		FEImGuiWindow::ContentObjects.clear();
		FEImGuiWindow::ContentObjectNames.clear();
		FEImGuiWindow::ContentObjectPaths.clear();
		FEImGuiWindow::ContentObjectTypes.clear();

	}
}