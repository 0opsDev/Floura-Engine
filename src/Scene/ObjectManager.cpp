#include "ObjectManager.h"
#include "IdManager.h"
#include "scene.h"
#include "LightingHandler.h"
#include <utils/logConsole.h>

void dupeModel(int index) {
	std::unique_ptr<ModelObject> newObject = std::make_unique<ModelObject>(); // Use std::make_unique
	newObject->IsLod = Scene::modelObjects[index]->IsLod;
	std::string name = Scene::modelObjects[index]->ObjectName + "_copy";
	newObject->transform = Scene::modelObjects[index]->transform;
	newObject->rotation = Scene::modelObjects[index]->rotation;
	newObject->scale = Scene::modelObjects[index]->scale;
	newObject->isCollider = Scene::modelObjects[index]->isCollider;
	newObject->DoCulling = Scene::modelObjects[index]->DoCulling;
	newObject->BoxColliderTransform = Scene::modelObjects[index]->BoxColliderTransform;
	newObject->BoxColliderScale = Scene::modelObjects[index]->BoxColliderScale;
	newObject->CullFrontFace = Scene::modelObjects[index]->CullFrontFace;
	newObject->LodCount = Scene::modelObjects[index]->LodCount;
	newObject->LodDistance = Scene::modelObjects[index]->LodDistance;
	newObject->CreateObject(Scene::modelObjects[index]->ModelPath, name, "Assets/Material/Default.Material");
	Scene::modelObjects.push_back(std::move(newObject)); // std::move is crucial here

	LogConsole::print("Created Model Object: " + name);
}

void billboardDuplicate(int index) {
	BillBoardObject newBillBoardObject; // Create a temporary BillBoardObject

	std::string name = Scene::BillBoardObjects[index].ObjectName + "_copy";
	newBillBoardObject.DoFrustumCull = Scene::BillBoardObjects[index].DoFrustumCull;
	newBillBoardObject.isCollider = Scene::BillBoardObjects[index].isCollider;
	newBillBoardObject.scale = Scene::BillBoardObjects[index].scale;
	newBillBoardObject.transform = Scene::BillBoardObjects[index].transform;
	newBillBoardObject.isCollider = Scene::BillBoardObjects[index].isCollider;
	newBillBoardObject.tickrate = Scene::BillBoardObjects[index].tickrate;
	newBillBoardObject.DoFrustumCull = Scene::BillBoardObjects[index].DoFrustumCull;
	newBillBoardObject.doUpdateSequence = Scene::BillBoardObjects[index].doUpdateSequence;
	newBillBoardObject.doPitch = Scene::BillBoardObjects[index].doPitch;
	newBillBoardObject.flag_isanimated = Scene::BillBoardObjects[index].flag_isanimated;
	newBillBoardObject.CreateObject(Scene::BillBoardObjects[index].path, name);
	Scene::BillBoardObjects.push_back(newBillBoardObject);
	LogConsole::print("Created BillBoard Object: " + name);
}

void colliderDuplicate(int index) {
	std::string name = Scene::CubeColliderObject[index].name + "_copy";
	CubeCollider newCubeColliderObject; // Create a temporary CubeCollider
	newCubeColliderObject.init();
	newCubeColliderObject.name = name;
	newCubeColliderObject.colliderRotation = Scene::CubeColliderObject[index].colliderRotation;
	newCubeColliderObject.colliderScale = Scene::CubeColliderObject[index].colliderScale;
	newCubeColliderObject.colliderXYZ = Scene::CubeColliderObject[index].colliderXYZ;
	newCubeColliderObject.enabled = Scene::CubeColliderObject[index].enabled;
	Scene::CubeColliderObject.push_back(newCubeColliderObject);
	LogConsole::print("Created CubeCollider Object: " + name);
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

	tempLight.ID.ObjType = 'l';
	tempLight.ID.index = LightingHandler::Lights.size();
	IdManager::AddID(tempLight.ID);

	LightingHandler::Lights.push_back(tempLight);

	LogConsole::print("Created LightObject");
}

void ObjectManager::duplicateObject(unsigned char ObjType, unsigned int UniqueNumber) {
	int index = IdManager::fetchIndexFromID(ObjType, UniqueNumber);
	if (index == -1) 
	{
		return;
	}
	else
	{
		switch (ObjType)
		{
		case 'm': // model
			dupeModel(index);
			break;
		case 'b': // billboard
			billboardDuplicate(index);
			break;
		case 'c': // collider
			colliderDuplicate(index);
			break;
		case 's': // sound
			soundDuplicate(index);
			break;
		case 'l': // light
			lightDuplicate(index);
			break;
		}
		//IdManager::lowestModelIndexSync();
		//IdManager::lowestBillBoardIndexSync();
		//IdManager::lowestColliderIndexSync();
		//IdManager::lowestSoundIndexSync();
		//IdManager::lowestLightIndexSync();
	}
}

void deleteObjectSwitch(unsigned char ObjType, int index)
{
	switch (ObjType)
	{
	case 'm': // model
		Scene::modelObjects[index]->Delete();
		Scene::modelObjects.erase(Scene::modelObjects.begin() + index);
		break;
	case 'b': // billboard
		Scene::BillBoardObjects[FEImGuiWindow::SelectedObjectIndex].Delete();
		Scene::BillBoardObjects.erase(Scene::BillBoardObjects.begin() + FEImGuiWindow::SelectedObjectIndex);
		break;
	case 'c': // collider
		Scene::CubeColliderObject[FEImGuiWindow::SelectedObjectIndex].Delete();
		Scene::CubeColliderObject.erase(Scene::CubeColliderObject.begin() + FEImGuiWindow::SelectedObjectIndex);
		break;
	case 'l':
		LightingHandler::deleteLight(FEImGuiWindow::SelectedObjectIndex);
		break;
	}
}

void ObjectManager::deleteObject(unsigned char ObjType, unsigned int UniqueNumber)
{
// fetch index from ID
	int index = IdManager::fetchIndexFromID(ObjType, UniqueNumber);
	if (index == -1) 
	{
		return;
	}
	else
	{
		deleteObjectSwitch(ObjType, index);

		IdManager::lowestModelIndexSync();

	}

}
void ObjectManager::deleteObjectwIndex(unsigned char ObjType, unsigned int index)
{
	deleteObjectSwitch(ObjType, index);

	IdManager::lowestModelIndexSync();
}

char ObjectManager::NameBuffer[256] = "New Object";

void ObjectManager::renameObject(unsigned char ObjType, unsigned int UniqueNumber, std::string newName)
{
	// fetch index from ID
	int index = IdManager::fetchIndexFromID(ObjType, UniqueNumber);
	if (index == -1)
	{
		return;
	}
	else
	{
		switch (ObjType)
		{
		case 'm': // model
			Scene::modelObjects[index]->ObjectName = newName;
			break;
		case 'b': // billboard
			Scene::BillBoardObjects[index].ObjectName = newName;
			break;
		case 'c': // collider
			Scene::CubeColliderObject[index].name = newName;
			break;
		case 's': // sound
			Scene::SoundObjects[index].name = newName;
			break;
//		case 'l': // light
//			LightingHandler::Lights[index].name = newName;
//			break;
		}
	}
}

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
		case 'm': // model
			Scene::modelObjects[index]->ObjectName = newName;
			break;
		case 'b': // billboard
			Scene::BillBoardObjects[index].ObjectName = newName;
			break;
		case 'c': // collider
			Scene::CubeColliderObject[index].name = newName;
			break;
		case 's': // sound
			Scene::SoundObjects[index].name = newName;
			break;
			//		case 'l': // light
			//			LightingHandler::Lights[index].name = newName;
			//			break;
		}
	}
}