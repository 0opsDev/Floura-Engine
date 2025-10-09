#include "BillboardObject.h"
#include <Scene/scene.h>
#include <utils/logConsole.h>

void BillBoardObject::CreateObject(std::string path, std::string ObjectName) {

	ID.ObjType = 'b';
	ID.index = Scene::BillBoardObjects.size();
	IdManager::AddID(ID);

	CubeCollider.init();
	BillBoardObject::ObjectName = ObjectName;
	BillBoardObject::path = path;

	if (flag_isanimated) { // takes json
		BillBoardRenderObject.initSeq(path);
	}
	else {
		BillBoardRenderObject.init(path);
	}
}

void BillBoardObject::UpdateCollider() {

	if (isCollider) {
		CubeCollider.colliderXYZ = transform;
		CubeCollider.colliderScale = scale;
	}
}
void BillBoardObject::UpdateCameraCollider() {

	if (isCollider) {
	CubeCollider.update();
	}
}
void BillBoardObject::draw() {
	if (DoFrustumCull) {

		if (Camera::isBoxInFrustum(glm::vec3(transform.x, transform.y, transform.z), glm::vec3(scale.x, scale.y, scale.z))) {
			if (doUpdateSequence && flag_isanimated) {
				BillBoardRenderObject.UpdateSequence(tickrate);
			}
			BillBoardRenderObject.draw(doPitch, transform.x, transform.y, transform.z, scale.x, scale.y, scale.z);
		}
	}
	else {
		if (doUpdateSequence && flag_isanimated) {
			BillBoardRenderObject.UpdateSequence(tickrate);
		}
		BillBoardRenderObject.draw(doPitch, transform.x, transform.y, transform.z, scale.x, scale.y, scale.z);
	}
	if (isCollider) {
		CubeCollider.draw();
	}
	
}

void BillBoardObject::Delete() {
	BillBoardRenderObject.Delete();
	//update lowest free index
	if (ID.index < IdManager::lowestDeletedIndex.BillBoard || IdManager::lowestDeletedIndex.BillBoard == -1) {
		IdManager::lowestDeletedIndex.BillBoard = ID.index;
		LogConsole::print("Lowest Deleted BillBoard Index is now: " + std::to_string(IdManager::lowestDeletedIndex.BillBoard));
	}

	IdManager::RemoveID(ID);
	//IdManager::lowestBillBoardIndexSync(); // sync up the index after deletion because the array has now changed
}