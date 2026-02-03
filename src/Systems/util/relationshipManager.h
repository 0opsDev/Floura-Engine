#ifndef RELATIONSHIP_MANAGER_H
#define RELATIONSHIP_MANAGER_H

#include <xhash>

class RelationshipManager
{
public:

	static void addParent(int index, uint64_t parentUUID);

	static void removeParent(int index);

	static int indexFromUUIDEntity(uint64_t UUID);

	static void eraseChildFromParent(int parentIndex, uint64_t childUUID);

	static void pushChildToParent(int parentIndex, uint64_t childUUID);

	static bool parentHasChild(int parentIndex, uint64_t childUUID);
};

#endif