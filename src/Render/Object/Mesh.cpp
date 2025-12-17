#include "Mesh.h"
#include <utils/logConsole.h>
#include <camera/Camera.h>
#include <limits>
#include <utils/FE_math.h>
#include <Render/Object/SkyBox.h>

void Mesh::create(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, std::vector<Texture>& textures)
{
    // err checking
    if (vertices.empty()) {
         LogConsole::print("mesh.cpp Vertices are empty");
    }
    if (indices.empty()) {
        LogConsole::print("mesh.cpp indices are empty");
    }
    if (textures.empty()) {
        LogConsole::print("mesh.cpp textures are empty");
    }

    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;
    /*
        for (size_t i = 0; i < textures.size(); i++)
    {
        std::cout << textures[i].unit << " unit" << std::endl;
        std::cout << textures[i].type << " type" << std::endl;
        std::cout << textures[i].path << " path" << std::endl;
    }
    */


    setupMesh();
}

void Mesh::createWithoutTexture(std::vector<Vertex>& vertices, std::vector<GLuint>& indices)
{
    // err checking
    if (vertices.empty()) {
        LogConsole::print("mesh.cpp Vertices are empty");
    }
    if (indices.empty()) {
        LogConsole::print("mesh.cpp indices are empty");
    }

    this->vertices = vertices;
    this->indices = indices;

    setupMesh();
}

void Mesh::draw(Shader& shader, glm::mat4 modelMatrix)
{
    shader.Activate();
    VAO.Bind();

    unsigned int numDiffuse = 0;
    unsigned int numSpecular = 0;
    unsigned int numNormal = 0;
    unsigned int numDisplacement = 0;

    for (unsigned int i = 0; i < textures.size(); i++)
    {
        std::string num;
        std::string type = textures[i].type;
        if (type == "texture_diffuse") {
            num = std::to_string(numDiffuse++);
        }
        else if (type == "texture_roughness") {
            num = std::to_string(numSpecular++);
        }
        else if (type == "texture_normal") {
            num = std::to_string(numNormal++);
        }
        else if (type == "texture_displacement"){
            num = std::to_string(numDisplacement++);
        }
        textures[i].texUnit(shader, (type + "0").c_str(), textures[i].unit);
        textures[i].Bind();
    }
    // Camera Matrix
    glUniform3f(glGetUniformLocation(shader.ID, "camPos"), Camera::Position.x, Camera::Position.y, Camera::Position.z);
    Camera::Matrix(shader, "camMatrix");

    // Push model matrix to the vertex shader
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glm::mat3 model3x3 = glm::mat3(modelMatrix);
    glm::mat3 normalMatrix = glm::transpose(glm::inverse(model3x3));
    glUniformMatrix3fv(glGetUniformLocation(shader.ID, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));

    //glEnable(GL_BLEND);
    //glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // Draw the mesh
    if (drawType == 0)
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    else if (drawType == 1)
        glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
	else if (drawType == 2)
    glDrawElements(GL_POINTS, indices.size(), GL_UNSIGNED_INT, 0);

    //glDisable(GL_BLEND);

    //glBindTexture(GL_TEXTURE_2D, 0);
}

void Mesh::setupMesh()
{
    VAO.Bind();

    VBO VBO(vertices);

    EBO EBO(indices);

    const size_t stride = sizeof(Vertex);

    // first is slot then how many varables, these are all vec3 and 2
    VAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, stride, (void*)offsetof(Vertex, position));
    VAO.LinkAttrib(VBO, 1, 3, GL_FLOAT, stride, (void*)offsetof(Vertex, normal));
    VAO.LinkAttrib(VBO, 2, 3, GL_FLOAT, stride, (void*)offsetof(Vertex, color));
    VAO.LinkAttrib(VBO, 3, 2, GL_FLOAT, stride, (void*)offsetof(Vertex, texUV));
    VAO.LinkAttrib(VBO, 4, 3, GL_FLOAT, stride, (void*)offsetof(Vertex, tangent));
    VAO.LinkAttrib(VBO, 5, 3, GL_FLOAT, stride, (void*)offsetof(Vertex, biTangent));
    VAO.Unbind();
    VBO.Unbind();
    EBO.Unbind();
}

void Mesh::Delete()
{
    VAO.Delete();

    for (size_t i = 0; i < textures.size(); i++)
    {
        textures[i].Delete();
    }
    textures.clear();

    vertices.clear();
    indices.clear();
}


glm::vec2 Mesh::findTwoFurthestVerticesX()
{
    // max and min points
    glm::vec3 minX = glm::vec3(std::numeric_limits<float>::max());
    glm::vec3 maxX = glm::vec3(std::numeric_limits<float>::lowest());
	

    for (size_t x = 0; x < vertices.size(); x++)
    {
		minX = (vertices[x].position.x < minX.x) ? vertices[x].position : minX;
		maxX = (vertices[x].position.x > maxX.x) ? vertices[x].position : maxX;
	}
	return glm::vec2(minX.x, maxX.x);
}

glm::vec2 Mesh::findTwoFurthestVerticesY()
{
    glm::vec3 minY = glm::vec3(std::numeric_limits<float>::max());
    glm::vec3 maxY = glm::vec3(std::numeric_limits<float>::lowest());
    for (size_t x = 0; x < vertices.size(); x++)
    {
        minY = (vertices[x].position.y < minY.y) ? vertices[x].position : minY;
        maxY = (vertices[x].position.y > maxY.y) ? vertices[x].position : maxY;
    }
	return glm::vec2(minY.y, maxY.y);
}
glm::vec2 Mesh::findTwoFurthestVerticesZ()
{
    glm::vec3 minZ = glm::vec3(std::numeric_limits<float>::max());
    glm::vec3 maxZ = glm::vec3(std::numeric_limits<float>::lowest());
    for (size_t x = 0; x < vertices.size(); x++)
    {
        minZ = (vertices[x].position.z < minZ.z) ? vertices[x].position : minZ;
        maxZ = (vertices[x].position.z > maxZ.z) ? vertices[x].position : maxZ;
    }
	return glm::vec2(minZ.z, maxZ.z);
}

Collision::AABB Mesh::createAABBfromMesh()
{
    // optimise later btw
    Collision::AABB AABB;

    /*
        // find largest distance on each axis
    glm::vec2 X = Mesh::findTwoFurthestVerticesX();
    glm::vec2 Y = Mesh::findTwoFurthestVerticesY();
    glm::vec2 Z = Mesh::findTwoFurthestVerticesZ();

    // find the centre in all 3 axis
    AABB.position = glm::vec3( // centre becomes location
        FE_Math::calculateCenter1D(X),
        FE_Math::calculateCenter1D(Y),
        FE_Math::calculateCenter1D(Z)
    );

    // find distance from furthest to the centre to each axis // edges to centre
    AABB.size = glm::vec3( // distance becomes size
        FE_Math::distanceFromTwoPoints1D(glm::vec2(AABB.position.x, FE_Math::furthestPoint(X))),
        FE_Math::distanceFromTwoPoints1D(glm::vec2(AABB.position.y, FE_Math::furthestPoint(Y))),
        FE_Math::distanceFromTwoPoints1D(glm::vec2(AABB.position.z, FE_Math::furthestPoint(Z)))
    );
    */

    glm::vec3 min = glm::vec3(std::numeric_limits<float>::max());
    glm::vec3 max = glm::vec3(std::numeric_limits<float>::lowest());

    for (size_t i = 0; i < vertices.size(); i++)
    {
        min = glm::min(min, vertices[i].position);
        max = glm::max(max, vertices[i].position);
    }
    AABB.position = (min + max) * 0.5f;
    AABB.size = (max - min) * 0.5f;
    //AABB.size = (max - min);
    
    
    return AABB;
}