#ifndef MESH_CLASS_H
#define MESH_CLASS_H

#include <string>
#include "Render/Buffer/VAO.h"
#include "Render/Buffer/EBO.h"
#include "Camera/Camera.h"
#include "Texture.h"
#include <json/json.hpp>

class Mesh
{
public:
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture> textures;
    // Store VAO in public so it can be used in the Draw function
    VAO VAO;
    glm::vec3 lastHit = glm::vec3(0);
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);

    // Initializes the mesh
    Mesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, std::vector<Texture>& textures);

    bool collide(glm::vec3 victimXYZ, glm::vec3 victimScale, std::string collidertype);

    // Draws the mesh
    void Draw(Shader& shader, glm::mat4 modelMatrix);

    void Delete();
};

#endif
