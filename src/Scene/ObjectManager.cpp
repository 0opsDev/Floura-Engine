#include "ObjectManager.h"
#include "scene.h"
#include "LightingHandler.h"
#include <utils/logConsole.h>

void deleteObjectSwitch(unsigned char ObjType, int index)
{
	switch (ObjType)
	{
	case 'o': // object
		Scene::entityObjects[FEImGuiWindow::SelectedObjectIndex]->Delete();
//		Scene::entityObjects.erase(Scene::entityObjects.begin() + FEImGuiWindow::SelectedObjectIndex);
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