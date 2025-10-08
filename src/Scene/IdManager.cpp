#include "IdManager.h"
#include <utils/logConsole.h>
#include <utils/init.h>

std::vector<IdManager::ID> IdManager::IDs;

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