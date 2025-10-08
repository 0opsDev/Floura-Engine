#include "IdManager.h"
#include <utils/logConsole.h>
#include <utils/init.h>
#include "scene.h"
#include "LightingHandler.h"

std::vector<IdManager::ID> IdManager::IDs;

IdManager::LowestDeletedIndex IdManager::lowestDeletedIndex = { -1, -1, -1, -1, -1 };

void IdManager::onSceneLoad() {

	clearAllIDs();
	if (init::LogALL || init::LogSystems) LogConsole::print("IdManager - Scene Load");
}

void IdManager::clearAllIDs()
{
	lowestDeletedIndex.Model = -1;
	lowestDeletedIndex.BillBoard = -1;
	lowestDeletedIndex.Collider = -1;
	lowestDeletedIndex.Sound = -1;
	lowestDeletedIndex.Light = -1;
	IDs.clear();
	if (init::LogALL || init::LogSystems) LogConsole::print("IdManager - Cleared all IDs");
}

void IdManager::update()
{
	// Not used currently
	// should be some form of inteval for performance reasons, for now this is fine
	lowestIndexSync();
}

void IdManager::lowestIndexSync()
{
	// check if index is not -1, first check all (ealy copout)
	if (lowestDeletedIndex.Model >= 0 || lowestDeletedIndex.BillBoard >= 0 || lowestDeletedIndex.Collider >= 0 || lowestDeletedIndex.Sound >= 0 || lowestDeletedIndex.Light >= 0)
	{
		// check which object types need to be synced
		// start the for at lowestDeletedIndex and go up until we find a free index

		// check if ID at index matches IdManagerID, then update the index, if not found,
		// set lowestDeletedIndex to -1, or if over the size of the array, otherwise if found, +lowestDeletedIndex by 1 and continue

		if (lowestDeletedIndex.Model >= 0)
		{
			// for models starting at lowestDeletedIndex.Model
			for (size_t ind = lowestDeletedIndex.Model; ind < Scene::modelObjects.size(); ind++)
			{
				// for all ids in IdManager
				for (size_t indMananger = 0; indMananger < IDs.size(); indMananger++)
				{
					// check if both ids match
					if (IDmatch(Scene::modelObjects[ind]->ID, IDs[indMananger]))
					{
						LogConsole::print("Model ID match at index: " + std::to_string(ind) + " with UniqueNumber: " + std::to_string(Scene::modelObjects[ind]->ID.UniqueNumber));
						// if both match, check if index is the same, if not, update it						
						if (Scene::modelObjects[ind]->ID.index != ind)
						{
							// update index
							Scene::modelObjects[ind]->ID.index = ind;
							LogConsole::print("Updated Model ID index to: " + std::to_string(ind) + " for UniqueNumber: " + std::to_string(Scene::modelObjects[ind]->ID.UniqueNumber));
						}
						// check if lowestDeletedIndex.Model is now out of bounds
						if (lowestDeletedIndex.Model >= static_cast<int>(Scene::modelObjects.size()))
						{
							lowestDeletedIndex.Model = -1;
							LogConsole::print("Lowest Deleted Model Index is now: " + std::to_string(lowestDeletedIndex.Model));
						}
						// increment lowestDeletedIndex.Model because both ids matched
						lowestDeletedIndex.Model++;
					}
				}
			}
		}
		if (lowestDeletedIndex.BillBoard >= 0)
		{
			for (size_t i = lowestDeletedIndex.BillBoard; i < Scene::BillBoardObjects.size(); i++)
			{
				// for all ids in IdManager
				for (size_t indMananger = 0; indMananger < IDs.size(); indMananger++)
				{
					// check if both ids match
					if (IDmatch(Scene::BillBoardObjects[i].ID, IDs[indMananger]))
					{
						LogConsole::print("BillBoard ID match at index: " + std::to_string(i) + " with UniqueNumber: " + std::to_string(Scene::BillBoardObjects[i].ID.UniqueNumber));
						// if both match, check if index is the same, if not, update it						
						if (Scene::BillBoardObjects[i].ID.index != i)
						{
							// update index
							Scene::BillBoardObjects[i].ID.index = i;
							LogConsole::print("Updated BillBoard ID index to: " + std::to_string(i) + " for UniqueNumber: " + std::to_string(Scene::BillBoardObjects[i].ID.UniqueNumber));
						}
						// check if lowestDeletedIndex.BillBoard is now out of bounds
						if (lowestDeletedIndex.BillBoard >= static_cast<int>(Scene::BillBoardObjects.size()))
						{
							lowestDeletedIndex.BillBoard = -1;
							LogConsole::print("Lowest Deleted BillBoard Index is now: " + std::to_string(lowestDeletedIndex.BillBoard));
						}
						// increment lowestDeletedIndex.BillBoard because both ids matched
						lowestDeletedIndex.BillBoard++;
					}
				}
			}
		}
		if (lowestDeletedIndex.Collider >= 0)
		{
			for (size_t i = lowestDeletedIndex.Collider; i < Scene::CubeColliderObject.size(); i++)
			{
				// for all ids in IdManager
				for (size_t indMananger = 0; indMananger < IDs.size(); indMananger++)
				{
					// check if both ids match
					if (IDmatch(Scene::CubeColliderObject[i].ID, IDs[indMananger]))
					{
						LogConsole::print("Collider ID match at index: " + std::to_string(i) + " with UniqueNumber: " + std::to_string(Scene::CubeColliderObject[i].ID.UniqueNumber));
						// if both match, check if index is the same, if not, update it						
						if (Scene::CubeColliderObject[i].ID.index != i)
						{
							// update index
							Scene::CubeColliderObject[i].ID.index = i;
							LogConsole::print("Updated Collider ID index to: " + std::to_string(i) + " for UniqueNumber: " + std::to_string(Scene::CubeColliderObject[i].ID.UniqueNumber));
						}
						// check if lowestDeletedIndex.Collider is now out of bounds
						if (lowestDeletedIndex.Collider >= static_cast<int>(Scene::CubeColliderObject.size()))
						{
							lowestDeletedIndex.Collider = -1;
							LogConsole::print("Lowest Deleted Collider Index is now: " + std::to_string(lowestDeletedIndex.Collider));
						}
						// increment lowestDeletedIndex.Collider because both ids matched
						lowestDeletedIndex.Collider++;
					}
				}
			}
		}
		if (lowestDeletedIndex.Sound >= 0)
		{
			// current not implemented
		}
		if (lowestDeletedIndex.Light >= 0)
		{
			for (size_t i = lowestDeletedIndex.Light; i < LightingHandler::Lights.size(); i++)
			{
				// for all ids in IdManager
				for (size_t indMananger = 0; indMananger < IDs.size(); indMananger++)
				{
					// check if both ids match
					if (IDmatch(LightingHandler::Lights[i].ID, IDs[indMananger]))
					{
						LogConsole::print("Light ID match at index: " + std::to_string(i) + " with UniqueNumber: " + std::to_string(LightingHandler::Lights[i].ID.UniqueNumber));
						// if both match, check if index is the same, if not, update it						
						if (LightingHandler::Lights[i].ID.index != i)
						{
							// update index
							LightingHandler::Lights[i].ID.index = i;
							LogConsole::print("Updated Light ID index to: " + std::to_string(i) + " for UniqueNumber: " + std::to_string(LightingHandler::Lights[i].ID.UniqueNumber));
						}
						// check if lowestDeletedIndex.Light is now out of bounds
						if (lowestDeletedIndex.Light >= static_cast<int>(LightingHandler::Lights.size()))
						{
							lowestDeletedIndex.Light = -1;
							LogConsole::print("Lowest Deleted Light Index is now: " + std::to_string(lowestDeletedIndex.Light));
						}
						// increment lowestDeletedIndex.Light because both ids matched
						lowestDeletedIndex.Light++;
					}
				}
			}
		}
	}
	else 
	{
		return;
	}
}

bool IdManager::IDmatch(IdManager::ID& ID1, IdManager::ID& ID2)
{
	// two ids, check if they match
	if (ID1.ObjType == ID2.ObjType && ID1.UniqueNumber == ID2.UniqueNumber)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void IdManager::AddID(IdManager::ID& checkID)
{
	//check if ID is empty
	if (checkID.UniqueNumber == 0)
	{
		// random number
		checkID.UniqueNumber = rand() % 10000 + 1; // random number between 1 and 10000

		if (init::LogALL || init::LogSystems) LogConsole::print("ID UniqueNumber was 0, assigned random number: " + std::to_string(checkID.UniqueNumber));

	}
	// check IDs with ID
	for (const auto& id : IDs) {
		if (id.ObjType == checkID.ObjType && id.UniqueNumber == checkID.UniqueNumber) {
			// ID exists
			// increment by 1 and try again
			checkID.UniqueNumber = rand() % 10000 + 1; // random number between 1 and 10000
			LogConsole::print("ID conflicting, incremented UniqueNumber to: " + std::to_string(checkID.UniqueNumber));
		}
	}

	// add ID to manager
	ID newID;
	newID.ObjType = checkID.ObjType;
	newID.UniqueNumber = checkID.UniqueNumber;
	newID.index = checkID.index;
	IDs.push_back(newID);
	if (init::LogALL || init::LogSystems) LogConsole::print("Checked/Added ID - Type: " + std::to_string(static_cast<int>(checkID.ObjType)) + ", uniqueNumber: " + std::to_string(checkID.UniqueNumber) + ", Total IDs: " + std::to_string(IDs.size()));
	if (init::LogALL || init::LogSystems) LogConsole::print("ID: " + std::to_string(checkID.ObjType) + "*" + std::to_string(checkID.UniqueNumber));
}

void IdManager::RemoveID(IdManager::ID& checkID)
{
	// find ID in manager
	for (auto it = IDs.begin(); it != IDs.end(); ++it) {
		if (it->ObjType == checkID.ObjType && it->UniqueNumber == checkID.UniqueNumber) {
			// ID found, remove it
			IDs.erase(it);
			if (init::LogALL || init::LogSystems) LogConsole::print("Removed ID - Type: " + std::to_string(static_cast<int>(checkID.ObjType)) + ", uniqueNumber: " + std::to_string(checkID.UniqueNumber) + ", Total IDs: " + std::to_string(IDs.size()));
			return;
		}
	}
	if (init::LogALL || init::LogSystems) LogConsole::print("ID to remove not found - Type: " + std::to_string(static_cast<int>(checkID.ObjType)) + ", uniqueNumber: " + std::to_string(checkID.UniqueNumber));
}