#include <Render/render_util/meshTools.h>

std::vector<GLuint> MeshTools::simplifyIndices(const std::vector<GLuint>& indices, const std::vector<Vertex>& vertices, float factor, float marginOfError)
{
    std::vector<GLuint> simplifiedIndices(indices.size());
    size_t nIndexCount2 = static_cast<size_t>(indices.size() * factor);
    float maxAllowedAmountOfError = marginOfError;   // 0.01f
    
    size_t nIndexCount = meshopt_simplify
    (
    simplifiedIndices.data(), // destination 
    indices.data(),
    indices.size(), // indice count
    &vertices[0].position.x, // pointer to first index of vertex's position
    vertices.size(), // vertex count
    sizeof(Vertex),
    nIndexCount2,
    maxAllowedAmountOfError,
    0,
    nullptr
    );
    
    simplifiedIndices.resize(nIndexCount);
    return simplifiedIndices;
}
