#ifndef LINE_3D_CLASS_H
#define LINE_3D_CLASS_H

#include <Render/Shader/shaderClass.h>
#include "Render/Buffer/VAO.h"
#include "Render/Buffer/EBO.h"

class Line3D {
public:

	Line3D(glm::vec3 pos1, glm::vec3 pos2);
	void updateVBO(glm::vec3 pos1, glm::vec3 pos2);
	~Line3D();
	void translate(glm::vec3 position, glm::vec3 scale, glm::vec3 rotation);
	void draw(glm::vec3 colour);

private:
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	VAO VAO;
	glm::mat4 lineMatrix = glm::mat4(0.0f);
};

#endif
