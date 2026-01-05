#ifndef MESH_CLASS_H
#define MESH_CLASS_H

#include <Render/Shader/shaderClass.h>
#include "string"
#include "iostream"
#include "Render/Buffer/VAO.h"
#include "Render/Buffer/EBO.h"
#include "Render/Object/Texture.h"
#include <xhash>
#include "camera/Camera.h"
class Mesh {
public:

    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture> textures;

    void create(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, std::vector<Texture>& textures);
    void draw(Shader &shader, Camera camera, glm::mat4 meshMatrix);

    void Delete();
    
private:

    VAO VAO;
    void setupMesh();
};

#endif
