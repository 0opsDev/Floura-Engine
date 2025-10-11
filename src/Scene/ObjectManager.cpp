#include "ObjectManager.h"
#include "IdManager.h"
#include "scene.h"
#include "LightingHandler.h"

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