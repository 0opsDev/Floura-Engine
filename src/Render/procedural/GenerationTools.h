#ifndef GENERATION_TOOL_CLASS_H
#define GENERATION_TOOL_CLASS_H

#include <vector>
#include "glm/glm.hpp"
#include "Render/Buffer/VBO.h"
#include "Render/Object/Mesh.h"

class GenerationTools {
public:
    
    struct chunk
    {
        glm::vec3 position = glm::vec3(0.0f);
        int areaXZ = 0;
        int LOD_LEVEL = 0;
        glm::vec3 colourLod = glm::vec3(1.0f);
        
        // put array of meshes on here when it comes to terrain
        
    };
    
    static Mesh generateSubdivededPlaneMesh(int div, int width);
    
    static std::vector<GLfloat> verySimplePlane(int div, float width);
    static std::vector<GLuint> genPlaneInd(int div);
    static std::vector<Vertex> GLfloatToVertex(std::vector<GLfloat> floats);
    
    static void tagChunkLODLevels(float transitionDistance, glm::vec3 vPosition, int maxLOD, std::vector<GenerationTools::chunk>& chunks);
    
};

#endif
