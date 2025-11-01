#ifndef SHAPE_3D_CLASS_H
#define SHAPE_3D_CLASS_H

#include <Render/Shader/shaderClass.h>
#include "Mesh.h"
#include <Core/Render.h>

class Shape3D {
public:

	RenderClass::transformation globalTransformation;

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
