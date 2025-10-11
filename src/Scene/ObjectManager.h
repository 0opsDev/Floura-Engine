#ifndef OBJECT_MANAGER_CLASS_H	
#define OBJECT_MANAGER_CLASS_H

#include <iostream>

class ObjectManager
{
public:
	//static void createObject(unsigned char ObjType);
	//static unsigned int createObjectAndFetchID(unsigned char ObjType);

	static void duplicateObject(unsigned char ObjType, unsigned int UniqueNumber);
	//static void duplicateObjectwIndex(unsigned char ObjType, unsigned int index);

	static void deleteObject(unsigned char ObjType, unsigned int UniqueNumber);
	static void deleteObjectwIndex(unsigned char ObjType, unsigned int index);

	static char NameBuffer[256];
	static void renameObject(unsigned char ObjType, unsigned int UniqueNumber, std::string newName);
	static void renameObjectwIndex(unsigned char ObjType, unsigned int index, std::string newName);

};
#endif // OBJECT_MANAGER_CLASS_H
