#include "BillboardObject.h"

void BillBoardObject::CreateObject(std::string type, std::string path, std::string ObjectName) {
	CubeCollider.init();
	BillBoardObject::type = type;
	BillBoardObject::ObjectName = ObjectName;
	BillBoardObject::path = path;
	if (type == "static" || type == "Static") // takes image
	{
		BillBoardRenderObject.init(path);
	}
	else if (type == "animated" || type == "Animated") { // takes json
		BillBoardRenderObject.initSeq(path);
	}
	else {
		std::cout << "[Billboard Object] no such type: " << type << std::endl;
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
			if (doUpdateSequence && type == "animated" || doUpdateSequence && type == "Animated") {
				BillBoardRenderObject.UpdateSequence(tickrate);
			}
			BillBoardRenderObject.draw(doPitch, transform.x, transform.y, transform.z, scale.x, scale.y, scale.z);
		}
	}
	else {
		if (doUpdateSequence && type == "animated" || doUpdateSequence && type == "Animated") {
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
}