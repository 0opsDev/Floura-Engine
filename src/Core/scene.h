#ifndef SCENE_CLASS_H	
#define SCENE_CLASS_H

#include <iostream>
#include "Gameplay/ModelObject.h"
#include "Gameplay/BillboardObject.h"
#include <Sound/SoundProgram.h>
#include <Sound/SoundRunner.h>

//testclass
class Scene
{
public:

	using json = nlohmann::json;

	static std::vector<ModelObject> modelObjects;
	static std::vector<BillBoardObject> BillBoardObjects;
	static std::vector <CubeCollider> CubeColliderObject;
	static std::vector <SoundProgram> SoundObjects;
	static std::vector <bool> isSoundLoop;

	static void LoadScene(std::string path);

	static void SaveScene(std::string path);

	static void initJsonModelLoad(std::string path);

	static void JsonModelSave(std::string path);
	
	static void JsonBillBoardSave(std::string path);

	static void JsonColliderSave(std::string path);

	static void AddSceneModelObject(std::string type, std::string path, std::string name);

	static void AddSceneBillBoardObject(std::string name, std::string type, std::string path);

	static void AddSceneSoundObject(std::string name, std::string path);

	static void AddSceneColliderObject(std::string name);

	static void initJsonBillBoardLoad(std::string path);

	static void initJsonColliderLoad(std::string path);

	static void initJsonSoundObjectLoad(std::string path);

	static void initJsonSettingsLoad(std::string path);

	static void Update();

	static void Delete();

private:
};
#endif