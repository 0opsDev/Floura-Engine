#ifndef ID_MANAGER_CLASS_H	
#define ID_MANAGER_CLASS_H

#include <iostream>
#include <variant>
#include <vector>

class IdManager
{
public:

	static struct LowestDeletedIndex { // -1 = none (for syncing up the tree ^^)
		int Model;
		int BillBoard;
		int Collider;
		int Sound;
		int Light;
	};

	static LowestDeletedIndex lowestDeletedIndex;

	static void onSceneLoad();
	static void clearAllIDs();
	static void update();
	static void lowestIndexSync(); // syncs up the lowest deleted index with the actual lowest deleted index in the array

	//static void LowestDeletedIndexSync();

	static struct ID{
		unsigned char ObjType; // object type identifier
		unsigned int UniqueNumber; // unique number for object
		unsigned int index; // index in array
	};

	static std::vector<ID> IDs;

	static bool IDmatch(IdManager::ID& ID1, IdManager::ID& ID2);

	// object interaction
	
	static void AddID(IdManager::ID& checkID); // looks for object type and index, then contacts the object and sets its ID to the index in the array
	static void RemoveID(IdManager::ID& checkID); // looks for object type and index, then contacts the object and removes its ID, then removes the ID from the array
	// update id info

	// scene

	//static void LoadIDs(std::string path);
	//static void SaveIDs(std::string path);

private:

	// delete all Ids

};
#endif // ID_MANAGER_CLASS_H