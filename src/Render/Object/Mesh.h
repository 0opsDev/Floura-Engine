#ifndef MESH_CLASS_H
#define MESH_CLASS_H

#include "Physics/Collision.h"
#include <Render/Shader/shaderClass.h>
#include "string"
#include "iostream"
#include "Render/Buffer/VAO.h"
#include "Render/Buffer/EBO.h"
#include "Render/Object/Texture.h"

class Mesh {
public:


    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture> textures;

    void create(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, std::vector<Texture>& textures);
    void createWithoutTexture(std::vector<Vertex>& vertices, std::vector<GLuint>& indices);
    void draw(Shader &shader, glm::mat4 modelMatrix);

    void Delete();

	glm::vec2 findTwoFurthestVerticesX();
	glm::vec2 findTwoFurthestVerticesY();
	glm::vec2 findTwoFurthestVerticesZ();

    Collision::AABB createAABBfromMesh();

	int drawType = 0; // 0 = triangles, 1 = lines, 2 = points

private:

    VAO VAO;
    void setupMesh();
};

#endif
