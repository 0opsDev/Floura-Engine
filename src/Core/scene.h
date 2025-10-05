#ifndef SCENE_CLASS_H	
#define SCENE_CLASS_H

#include <iostream>
#include "Gameplay/ModelObject.h"
#include "Gameplay/BillboardObject.h"
#include <Sound/SoundProgram.h>
#include <Sound/SoundRunner.h>


using json = nlohmann::json;

//testclass
class Scene
{
public:

	static std::string sceneName; // Map loading

	static std::vector<std::unique_ptr<ModelObject>> modelObjects;
	static std::vector<BillBoardObject> BillBoardObjects;
	static std::vector <CubeCollider> CubeColliderObject;
	static std::vector <SoundProgram> SoundObjects;
	static std::vector <bool> isSoundLoop;

	// 4th component is radius

	static glm::vec3 initalCameraPos;

	static void init();

	static void LoadScene(std::string path);

	static void SaveScene(std::string path);

	static void JsonEnviromentSave(std::string path);

	static void JsonEnviromentLoad(std::string path);

	static void initJsonModelLoad(std::string path);

	static void JsonModelSave(std::string path);
	
	static void JsonBillBoardSave(std::string path);

	static void JsonColliderSave(std::string path);

	static void JsonSettingsSave(std::string path);

	static void JsonCameraSettingsSave(std::string path);

	static void AddSceneModelObject(bool type, std::string path, std::string name);

	static void AddSceneBillBoardObject(std::string name, bool isAnimated, std::string path);

	static void AddSceneSoundObject(std::string name, std::string path);

	static void AddSceneColliderObject(std::string name);

	static void initJsonBillBoardLoad(std::string path);

	static void initJsonColliderLoad(std::string path);

	static void initJsonSoundObjectLoad(std::string path); 

	static void initJsonSettingsLoad(std::string path);

	static void initCameraSettingsLoad(std::string path);

	static void Update();

	static void Delete();

private:
};
#endif