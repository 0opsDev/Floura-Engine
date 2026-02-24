#ifndef MESH_CLASS_H
#define MESH_CLASS_H

#include "Systems/Physics/Collision.h"
#include <Render/Shader/shaderClass.h>
#include "string"
#include "iostream"
#include "Render/Buffer/VAO.h"
#include "Render/Buffer/EBO.h"
#include "Render/Object/Texture.h"
#include <xhash>
#include <camera/Camera.h>

class Mesh {
public:

    uint64_t UUID;

    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture> textures;

    void create(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, std::vector<Texture>& textures);
    void createWithoutTexture(std::vector<Vertex>& vertices, std::vector<GLuint>& indices);
    void draw(Shader &shader, Camera Camera);

    void drawInstanced(Shader& shader, Camera Camera, int instanceCount);

    void Delete();

    void updateMatrix(glm::mat4 matrix);
	//void updatePrevMatrix(glm::mat4 matrix);
    void updatePosition(glm::vec3 position);
	void updateRotation(glm::vec3 rotation);
	void updateScale(glm::vec3 scale);
	void updateGlobalMatrix(glm::mat4 matrix);
	void updatePrevGlobalMatrix(glm::mat4 matrix);
    void updateGlobalPosition(glm::vec3 position);
    void updateGlobalScale(glm::vec3 scale);
    void updateGlobalRotation(glm::vec3 rotation);

	int drawType = 0; // 0 = triangles, 1 = lines, 2 = points

    std::string name = "empty";
    
private:
    
    glm::mat4 meshMatrix = glm::mat4(1.0f);
	//glm::mat4 meshPrevMatrix = glm::mat4(1.0f);
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
    glm::mat4 globalMeshMatrix = glm::mat4(1.0f);
	glm::mat4 globalPrevMeshMatrix = glm::mat4(1.0f);
    glm::vec3 globalPosition = glm::vec3(0.0f);
    glm::vec3 globalScale = glm::vec3(1.0f);
    glm::vec3 globalRotation = glm::vec3(0.0f, 0.0f, 0.0f);

    VAO VAO;
    void setupMesh();
};

#endif
