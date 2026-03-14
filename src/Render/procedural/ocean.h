#ifndef OCEAN_CLASS_H
#define OCEAN_CLASS_H

#include <Render/Shader/shaderClass.h>
#include  "Render/Object/Mesh.h"
#include "Render/Buffer/VBO.h"
#include "Render/Cube/CubeVisualizer.h"
#include "Render/procedural/GenerationTools.h"

class ocean {
public:
    uint64_t UUID;

    struct transformation {
        glm::vec3 position = glm::vec3(0.0f);
        glm::vec3 rotation = glm::vec3(0.0f);
        glm::quat qRotation = glm::quat(0.0f, 0.0f, 0.0f, 0.0f);
        glm::vec3 scale = glm::vec3(1.0f);
    };
    
    std::vector<GenerationTools::chunk> chunks;
    int maxLOD = 7;
    
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
    
    ocean(float radius,  float width);
    void assignUUID(uint64_t input);
    ~ocean();
    void tagChunkLODLevels(float transitionDistance, glm::vec3 vPosition); // should be rand regularly, also peak variable name
    
    void draw(Shader shader, Camera Camera);
    void dbgChunkDraw();
    
private:
    std::vector<Mesh> pMeshes;
    void generateChunks(int radius, int stepsize);
};

#endif
