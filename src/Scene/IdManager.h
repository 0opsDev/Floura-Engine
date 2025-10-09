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
	static void lowestModelIndexSync();
	static void lowestBillBoardIndexSync();
	static void lowestColliderIndexSync();
	static void lowestSoundIndexSync();
	static void lowestLightIndexSync();

	/*
	Model
	BillBoard
	Collider
	Sound
	Light
	*/

	//static void LowestDeletedIndexSync();

	struct ObjectID { // used for parent/child relationships
		unsigned char ObjType; // object type identifier
		unsigned int UniqueNumber; // unique number for object
	};

	struct ID{
		unsigned char ObjType; // object type identifier
		unsigned int UniqueNumber; // unique number for object
		unsigned int index; // index in array

		//Parent/Child relationships
		ObjectID parentID; // parent object id // should check if type is w for world, if so we dont need unique number
		std::vector<ObjectID> childIDs; // child object ids
	};

	static std::vector<ID> IDs;

	// used to check if two ids match
	static bool IDmatch(IdManager::ID& ID1, IdManager::ID& ID2);

	// used to check if the local ids index matches with the manager id index, very basic, recommend the advanced check below
	static bool doubleIdIndexVerify(IdManager::ID& ID);

	// returns index of IdManager ID Index that matches, or -1 if not found
	static int findIdManagerIndex(IdManager::ID& ID); 


	// should find the Manager ID that matches, then run the entire array of objects of that type to see if the index is valid between the two IDs
	// this is more expensive but more reliable
	//static bool advancedIndexVerify(IdManager::ID& ID); // checks if the index is valid in the array (needs to act different based on object type)

	// object interaction
	
	static void AddID(IdManager::ID& checkID); // looks for object type and index, then contacts the object and sets its ID to the index in the array
	static void RemoveID(IdManager::ID& checkID); // looks for object type and index, then contacts the object and removes its ID, then removes the ID from the array
	// update id info

	// scene

	//static void LoadIDs(std::string path);
	//static void SaveIDs(std::string path); << chache and debug maybe? maybe for speed when reloading scene? idk

private:

};
#endif // ID_MANAGER_CLASS_H