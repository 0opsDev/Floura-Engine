#ifndef ID_MANAGER_CLASS_H	
#define ID_MANAGER_CLASS_H

#include <iostream>
#include <variant>
#include <vector>

class IdManager
{
public:

	static struct ID{
		unsigned char ObjType;
		unsigned int ObjIndex;
		unsigned int UniqueNumber;
	};

	static std::vector<ID> IDs;

	// object interaction
	
	static void AddID(unsigned char type, unsigned int index, unsigned int uniqueNumber); // looks for object type and index, then contacts the object and sets its ID to the index in the array
	//static void ChangeID(unsigned char currentType, unsigned int currentIndex, unsigned char newType, unsigned int newIndex); // change the ID at the array index to a new type and index
	//static void RemoveID(unsigned char type, unsigned int index); // remove the ID from the array, and set the object ID to -1

	// scene

	//static void LoadIDs(std::string path);
	//static void SaveIDs(std::string path);

private:

	// delete all Ids

};
#endif // ID_MANAGER_CLASS_H