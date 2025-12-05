#ifndef SCENE_CLASS_H	
#define SCENE_CLASS_H

#include <iostream>
#include <Sound/SoundProgram.h>
#include <Sound/SoundRunner.h>
#include <Scene/Object/Entity.h>
#include <Physics/CubeCollider.h>
#include <thread>
#include "Physics/phyworld.h"
using json = nlohmann::json;

//testclass
class Scene
{
public:

	static std::string sceneName; // Map loading
	static std::vector <CubeCollider> CubeColliderObject;
	static std::vector <SoundProgram> SoundObjects;
	static std::vector <std::unique_ptr<entity>> entityObjects;
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

	static void AddSceneSoundObject(std::string name, std::string path);

	static void AddSceneColliderObject(std::string name);

	static void AddEntityObject(char type, std::string name, std::string path);

	static void initJsonBillBoardLoad(std::string path);

	static void initJsonColliderLoad(std::string path);

	static void initJsonSoundObjectLoad(std::string path); 

	static void JsonSoundObjectSave(std::string path);

	static void initJsonSettingsLoad(std::string path);

	static void initCameraSettingsLoad(std::string path);

	static void draw();

	static void Update();

	static void Delete();

private:
};
#endif