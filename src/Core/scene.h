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

	std::vector<ModelObject> modelObjects;
	std::vector<BillBoardObject> BillBoardObjects;
	std::vector <CubeCollider> CubeColliderObject;
	std::vector <SoundProgram> SoundObjects;
	std::vector <bool> isSoundLoop;

	Scene();

	void LoadScene(std::string path);

	void SaveScene(std::string path);

	void initJsonModelLoad(std::string path);

	void JsonModelSave(std::string path);

	void AddSceneModelObject(std::string type, std::string path, std::string name);

	void initJsonBillBoardLoad(std::string path);

	void initJsonColliderLoad(std::string path);

	void initJsonSoundObjectLoad(std::string path);

	void initJsonSettingsLoad(std::string path);

	void Update();

	void Delete();

private:
};
#endif