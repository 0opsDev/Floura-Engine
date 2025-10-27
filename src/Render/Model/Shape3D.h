#ifndef SHAPE_3D_CLASS_H
#define SHAPE_3D_CLASS_H

#include <Render/Shader/shaderClass.h>
#include "Mesh.h"

class Shape3D {
public:
	glm::vec3 gPosition = glm::vec3(0.0f), gRotation = glm::vec3(0.0f), gScale = glm::vec3(1.0f);

	void updatePosition(glm::vec3 Position);

	void updateRotation(glm::vec3 Rotation);

	void updateScale(glm::vec3 Scale);

	void create();

	void draw(Shader& shader);

	void Delete();

private:
	void loadModel(std::string path);
	Mesh shapeMesh;
};

#endif
