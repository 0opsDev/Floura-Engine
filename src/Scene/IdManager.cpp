#include "IdManager.h"
#include <utils/logConsole.h>
#include <utils/init.h>

std::vector<IdManager::ID> IdManager::IDs;

void IdManager::AddID(unsigned char type, unsigned int index, unsigned int uniqueNumber)
{
	// check if object already has an ID
	// check if ID already exists in array
	// check if type and index are valid

	ID newID;
	newID.ObjType = type;
	newID.ObjIndex = index;
	IDs.push_back(newID);

	// push id onto object

	// debug
	if (init::LogALL || init::LogSystems) LogConsole::print("Added ID - Type: " + std::to_string(static_cast<int>(type)) + ", Index: " + std::to_string(index) + ", Total IDs: " + std::to_string(IDs.size()));
	if (init::LogALL || init::LogSystems) LogConsole::print("ID: " + std::to_string(type) + "*" + std::to_string(index));
}