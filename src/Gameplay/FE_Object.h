#include <string>
#include "ModelObject.h"
#include "BillboardObject.h"
#ifndef FE_OBJECT_H
#define FE_OBJECT_H

//
//holds all the objects until i can merge them all into one giant class
// 
// just a place holder for now


class FE_Object
{
public:

	int ID;

	void create(const std::string& type, const std::string& name, const std::string& path, const std::string& Material);

	void update();

	void draw();

	void updateLights();

	void Delete();

	// getters
	std::string fetchName();
	std::string fetchType();
	std::string fetchPath();
	bool fetchDoFrustumCull();
	bool fetchIsCollider();

	//	setters
	void setName(const std::string& name);
	void setFlagIsLOD(bool flag);
	void setFlagIsAnimated(bool flag);
	void setDoFrustumCull(bool flag);
	void setIsCollider(bool flag);

	glm::vec3 gPosition = glm::vec3(0.0f,0.0f,0.0f);
	glm::vec3 gScale = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 gRotation = glm::vec3(0.0f, 0.0f, 0.0f);

private:

	glm::vec3 lPosition;
	glm::vec3 lScale;
	glm::vec3 lRotation;

	ModelObject ModelObject;
	BillBoardObject BillBoardObject;

	bool flag_ModelIsLOD = false;
	bool flag_BillBoardAnimated = false;

	std::string name;
	std::string type;
	std::string path;

};

#endif // FE_OBJECT_H