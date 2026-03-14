#ifndef TERRAIN_CLASS_H
#define TERRAIN_CLASS_H

#include <Render/Shader/shaderClass.h>
#include  "Render/Object/Mesh.h"
#include "Render/Buffer/VBO.h"
#include "Render/Cube/CubeVisualizer.h"

class Terrain {
public:
    uint64_t UUID;

    struct transformation {
        glm::vec3 position = glm::vec3(0.0f);
        glm::vec3 rotation = glm::vec3(0.0f);
        glm::quat qRotation = glm::quat(0.0f, 0.0f, 0.0f, 0.0f);
        glm::vec3 scale = glm::vec3(1.0f);
    };
    
    transformation globalTransformation;
    transformation prevGlobalTransformation;
    glm::mat4 gMatrix;
    glm::mat4 pgMatrix;
    
    void updatePosition(glm::vec3 Position);
    void updateRotation(glm::vec3 Rotation);
    void updateScale(glm::vec3 Scale);
    void updateTranformation();
    
    void updatePrevPosition(glm::vec3 Position);
    void updatePrevRotation(glm::vec3 Rotation);
    void updatePrevScale(glm::vec3 Scale);
    void updatePrevTranformation();
    
    Terrain(float div,  float width);
    void assignUUID(uint64_t input);
    ~Terrain();
    void draw(Shader shader, Camera Camera);
private:
    
    Mesh tMesh;
};

#endif
