#include "GenerationTools.h"


Mesh GenerationTools::generateSubdivededPlaneMesh(int div, int width)
{
    Mesh tMesh;
    std::vector<GLfloat> vsp;
    std::vector<GLuint> ind;
    std::vector<Vertex> vertices;
    
    vsp = GenerationTools::verySimplePlane(div, width);
    ind = GenerationTools::genPlaneInd(div);
    vertices = GenerationTools::GLfloatToVertex(vsp);
    
    tMesh.createWithoutTexture(vertices,ind);
    
    return tMesh;
}

std::vector<GLfloat> GenerationTools::verySimplePlane(int div, float width)
{
    std::vector<GLfloat> plane;
    float triangleSide = width / (float)div;
    float offset = width / 2.0f;

    // Use <= div to get the last edge of vertices
    for (int row = 0; row <= div; row++) 
    {
        for (int col = 0; col <= div; col++)
        {
            plane.push_back((col * triangleSide) - offset); // X
            plane.push_back(0.0f);               // Y
            plane.push_back((row * -triangleSide) + offset);// Z
        }
    }
    return plane;
}

std::vector<GLuint> GenerationTools::genPlaneInd(int div)
{
    std::vector<GLuint> indices;
    int numVertsPerRow = div + 1;

    for (int row = 0; row < div; row++) // row < div because we are counting squares
    {
        for (int col = 0; col < div; col++)
        {
            int topLeft = row * numVertsPerRow + col;
            int topRight = topLeft + 1;
            int bottomLeft = (row + 1) * numVertsPerRow + col;
            int bottomRight = bottomLeft + 1;

            // Triangle 1
            indices.push_back(topLeft);
            indices.push_back(bottomRight);
            indices.push_back(bottomLeft);

            // Triangle 2
            indices.push_back(topLeft);
            indices.push_back(topRight);
            indices.push_back(bottomRight);
        }
    }
    return indices;
}

std::vector<Vertex> GenerationTools::GLfloatToVertex(std::vector<GLfloat> floats)
{
    std::vector<Vertex> vertices;
    for (int i = 0; i < floats.size(); i +=3)
    {
        Vertex nVertex;
        nVertex.position = glm::vec3(floats[i], floats[i + 1], floats[i + 2]);
        nVertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
        nVertex.color = glm::vec3(0.0, 1.0, 0.0);
        vertices.push_back(nVertex);
    }
    return vertices;
}

void GenerationTools::tagChunkLODLevels(float transitionDistance,
    glm::vec3 vPosition, int maxLOD, std::vector<GenerationTools::chunk>& chunks)
{
    for (int i = 0; i < chunks.size(); ++i)
    {
        float distance = glm::distance(chunks[i].position, vPosition);
        int targetLOD = static_cast<int>(distance / transitionDistance);
        if (targetLOD > maxLOD) targetLOD = maxLOD;
        chunks[i].LOD_LEVEL = targetLOD;
        float colorShift = float(targetLOD) * 0.2f; 
        chunks[i].colourLod = glm::vec3(colorShift, 1.0f - (colorShift * 0.5f), colorShift);
    }
}
