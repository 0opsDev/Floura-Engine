#ifndef MESH_TOOLS_CLASS_H
#define MESH_TOOLS_CLASS_H

#include <Libraries/include/meshoptimizer/meshoptimizer.h>
#include <Render/Buffer/VBO.h>

class MeshTools {
public:
    
    //    std::vector<Vertex> vertices;
    //     std::vector<GLuint> indices;
    
    static std::vector<GLuint> simplifyIndices(const std::vector<GLuint>& indices, const std::vector<Vertex>& vertices, float factor, float marginOfError); 
   
private:

};

#endif
