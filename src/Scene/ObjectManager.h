#ifndef OBJECT_MANAGER_CLASS_H	
#define OBJECT_MANAGER_CLASS_H

#include <iostream>

class ObjectManager
{
public:

	static void deleteObjectwIndex(unsigned char ObjType, unsigned int index);

	static char NameBuffer[256];

};
#endif // OBJECT_MANAGER_CLASS_H
