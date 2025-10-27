#ifndef MESH_CLASS_H
#define MESH_CLASS_H


#include <Render/Shader/shaderClass.h>
#include "string"
#include "iostream"
#include "Render/Buffer/VAO.h"
#include "Render/Buffer/EBO.h"
#include "Texture.h"

class Mesh {
public:

    glm::vec3 lPosition = glm::vec3(0.0f);
    glm::vec3 lRotation = glm::vec3(0.0f);
    glm::vec3 lScale = glm::vec3(1.0f);

    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture> textures;

    void create(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, std::vector<Texture>& textures);
    void draw(Shader &shader, glm::mat4 modelMatrix);

    void Delete();

private:

    VAO VAO;
    void setupMesh();
};

#endif
