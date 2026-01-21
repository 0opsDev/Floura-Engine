#include "ObjectManager.h"
#include "scene.h"
#include "LightingHandler.h"
#include <utils/logConsole.h>

void entityDuplicate(int index) {
	std::unique_ptr<entity> newEntity = std::make_unique<entity>(); // Create a temporary BillBoardObject

	std::string name = Scene::entityObjects[index]->name + "_copy";

	if (Scene::entityObjects[index]->fetchType() == 'm') {
		newEntity->create(Scene::entityObjects[index]->fetchType(),
			name,
			Scene::entityObjects[index]->fetchPath(),
			"Assets/Material/Default.Material");
	}
	else if (Scene::entityObjects[index]->fetchType() == 'b') {
		newEntity->create(Scene::entityObjects[index]->fetchType(),
			name,
			Scene::entityObjects[index]->fetchPath(),
			"Assets/Material/Default.Material");

		newEntity->component.render.BillBoard->doPitch =Scene::entityObjects[index]->component.render.BillBoard->doPitch;
	}
	newEntity->setPosition(Scene::entityObjects[index]->fetchPosition());
	newEntity->setRotation(Scene::entityObjects[index]->fetchRotation());
	newEntity->setScale(Scene::entityObjects[index]->fetchScale());
	newEntity->component.systems.material.Material = Scene::entityObjects[index]->component.systems.material.Material;


	Scene::entityObjects.push_back(std::move(newEntity));
	LogConsole::print("cloned Entity: " + name);
}

void soundDuplicate(int index) {
	// To be implemented
}

void lightDuplicate(int index) {
	LightingHandler::Light tempLight;

	tempLight.position = LightingHandler::Lights[index].position;
	tempLight.rotation = LightingHandler::Lights[index].rotation;
	tempLight.colour = LightingHandler::Lights[index].colour;
	tempLight.radius = LightingHandler::Lights[index].radius;
	tempLight.type = LightingHandler::Lights[index].type;
	tempLight.enabled = LightingHandler::Lights[index].enabled;

	//tempLight.ID.ObjType = 'l';
	//tempLight.ID.index = LightingHandler::Lights.size();

	LightingHandler::Lights.push_back(tempLight);

	LogConsole::print("Created LightObject");
}

void deleteObjectSwitch(unsigned char ObjType, int index)
{
	switch (ObjType)
	{
	case 'o': // object
		Scene::entityObjects[FEImGuiWindow::SelectedObjectIndex]->Delete();
		Scene::entityObjects.erase(Scene::entityObjects.begin() + FEImGuiWindow::SelectedObjectIndex);
		break;
	case 'l':
		LightingHandler::deleteLight(FEImGuiWindow::SelectedObjectIndex);
		break;
	case 's': // sound
		Scene::SoundObjects[FEImGuiWindow::SelectedObjectIndex].DeleteSound();
		Scene::SoundObjects.erase(Scene::SoundObjects.begin() + FEImGuiWindow::SelectedObjectIndex);
		break;
	}
}

void ObjectManager::deleteObjectwIndex(unsigned char ObjType, unsigned int index)
{
	deleteObjectSwitch(ObjType, index);
}

char ObjectManager::NameBuffer[256] = "New Object";

void ObjectManager::renameObjectwIndex(unsigned char ObjType, unsigned int index, std::string newName)
{
	if (index == -1)
	{
		return;
	}
	else
	{
		switch (ObjType)
		{
		case 'o': // object
			Scene::entityObjects[index]->name = newName;
			break;
		case 's': // sound
			Scene::SoundObjects[index].name = newName;
			break;
		}
	}
}