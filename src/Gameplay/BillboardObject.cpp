#include "BillboardObject.h"

void BillBoardObject::CreateObject(std::string type, std::string path, std::string ObjectName) {
	CubeCollider.init();
	BillBoardObject::type = type;
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
	switch (isCollider) {
		case true: {
			break;
		}
		case false: {

			break;
		}
	}
	CubeCollider.colliderXYZ = transform;
	CubeCollider.colliderScale = scale;
}
void BillBoardObject::UpdateCameraCollider() {
	CubeCollider.update();
}
void BillBoardObject::draw() {
	if (doUpdateSequence && type == "animated" || doUpdateSequence && type == "Animated") {
		BillBoardRenderObject.UpdateSequence(tickrate);
	}
	BillBoardRenderObject.draw(doPitch, transform.x, transform.y, transform.z, scale.x, scale.y, scale.z);
	CubeCollider.draw();
}

void BillBoardObject::Delete() {
	BillBoardRenderObject.Delete();
}