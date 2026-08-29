#include "relationshipManager.h"
#include "Scene/scene.h"

void RelationshipManager::addParent(int index, uint64_t parentUUID){
	// child UUID
	uint64_t childUUID = Scene::entityObjects[index]->UUID;

	// self attach fail safe
	if (childUUID == parentUUID) return;

	// find parent index
	int newParentIndex = indexFromUUIDEntity(parentUUID);

	// check if child has parent attached to it as a child
	//if so erase the parent as a child
	if (newParentIndex != -1 && parentHasChild(index, parentUUID)){
		// erase parent as child of the child
		eraseChildFromParent(index, parentUUID);

		// now we must remove the child as a parent on the parent
		Scene::entityObjects[newParentIndex]->component.relationship.hasParent = false;
		Scene::entityObjects[newParentIndex]->component.relationship.parentUUID = 0;
	}

	// check if parent UUID already exisits on enity
	// if so find parent and erase child

	bool existingHasParent = Scene::entityObjects[index]->component.relationship.hasParent; // already have a parent
	uint64_t exisitingParentUUID = Scene::entityObjects[index]->component.relationship.parentUUID;

	// if there was an existing parent, erase the child from the parent
	if (existingHasParent){
		int exisitingParentIndex = indexFromUUIDEntity(exisitingParentUUID);
		
		// erase child
		if (exisitingParentIndex != -1) eraseChildFromParent(exisitingParentIndex, childUUID);
	}

	// incase the check below fails
	Scene::entityObjects[index]->component.relationship.hasParent = false;
	Scene::entityObjects[index]->component.relationship.parentUUID = 0;


	if (newParentIndex != -1) {

		//push child uuid to parents child array
		pushChildToParent(newParentIndex, childUUID);

		// add parent to entity at index
		Scene::entityObjects[index]->component.relationship.parentUUID = parentUUID;
		Scene::entityObjects[index]->component.relationship.hasParent = true;
	}

}

void RelationshipManager::removeParent(int index){
	if (index < 0 || index >= (int)Scene::entityObjects.size()) return;

	if (Scene::entityObjects[index]->component.relationship.hasParent){
		// goto the parent and remove the child
		uint64_t parentUUID = Scene::entityObjects[index]->component.relationship.parentUUID;

		// return if same
		//if (Scene::entityObjects[index]->UUID == parentUUID) return; // i dont think i need this

		// fetch parent index
		int parentIndex = indexFromUUIDEntity(parentUUID);

		// erase the child from the parent
		if (parentIndex != -1) eraseChildFromParent(parentIndex, Scene::entityObjects[index]->UUID);
	}
	// remove the parent from the child
	Scene::entityObjects[index]->component.relationship.hasParent = false;
	Scene::entityObjects[index]->component.relationship.parentUUID = 0;
}

int RelationshipManager::indexFromUUIDEntity(uint64_t UUID){
	for (size_t i = 0; i < Scene::entityObjects.size(); i++){
		if (Scene::entityObjects[i]->UUID == UUID) return i;
	}

	return -1;
}

void RelationshipManager::eraseChildFromParent(int parentIndex, uint64_t childUUID){
	for (size_t i = 0; i < Scene::entityObjects[parentIndex]->component.relationship.childUUID.size(); i++){
		// match
		if (Scene::entityObjects[parentIndex]->component.relationship.childUUID[i] == childUUID){
			Scene::entityObjects[parentIndex]->component.relationship.childUUID.erase(Scene::entityObjects[parentIndex]->component.relationship.childUUID.begin() + i);
			return;
		}
	}
}

void RelationshipManager::pushChildToParent(int parentIndex, uint64_t childUUID){
	if (!parentHasChild(parentIndex, childUUID) ) Scene::entityObjects[parentIndex]->component.relationship.childUUID.push_back(childUUID);
}

bool RelationshipManager::parentHasChild(int parentIndex, uint64_t childUUID){
	for (size_t i = 0; i < Scene::entityObjects[parentIndex]->component.relationship.childUUID.size(); i++){
		if (Scene::entityObjects[parentIndex]->component.relationship.childUUID[i] == childUUID){
			return true;
		}
	}
	return false;
}
