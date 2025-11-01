#ifndef BILLBOARD_OBJECT_CLASS_H
#define BILLBOARD_OBJECT_CLASS_H
#include "Physics/CubeCollider.h"
#include <glm/ext/quaternion_float.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#include <string>
#include <iostream>
#include "Render/Object/Billboard.h"
#include <camera/Camera.h>
#include <Render/Cube/CubeVisualizer.h>
#include <Scene/IdManager.h>

class BillBoardObject
{
public:
	CubeCollider CubeCollider;
	IdManager::ID ID;
	BillBoard BillBoardRenderObject;
	std::string ObjectName;
	bool flag_isanimated = false;
	std::string path;
	int tickrate = 12;
	bool doUpdateSequence = true;
	bool doPitch = true;
	bool isCollider = false;
	bool DoFrustumCull = true;
	glm::vec3 transform = glm::vec3(0,0,0);
	glm::vec3 scale = glm::vec3(1, 1, 1);

	void CreateObject(std::string path, std::string ObjectName);

	void UpdateCollider();

	void UpdateCameraCollider();

	void draw();

	void Delete();
};

#endif // BILLBOARD_OBJECT_CLASS_H