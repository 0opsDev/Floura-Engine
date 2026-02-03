#include "Mesh.h"
#include <utils/logConsole.h>
#include <limits>
#include <utils/FE_math.h>
#include <Render/Object/SkyBox.h>
#include "Scene/scene.h"
#include "Systems/util/UUID.h"
#include <Systems/Physics/BVH.h>

void Mesh::create(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, std::vector<Texture>& textures)
{
    UUID = UUID::returnHandle();

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

void Mesh::draw(Shader& shader, Camera Camera) // Scene::maincamera
{
    shader.Activate();
    VAO.Bind();

    unsigned int numDiffuse = 0;
    unsigned int numSpecular = 0;
    unsigned int numNormal = 0;
    unsigned int numDisp = 0;

    for (unsigned int i = 0; i < textures.size(); i++)
    {
        std::string num;
        std::string type = textures[i].type;
        if (type == "texture_diffuse") {num = std::to_string(numDiffuse++);}
        else if (type == "texture_roughness") {num = std::to_string(numSpecular++);}
        else if (type == "texture_normal") {num = std::to_string(numNormal++);}
        //else if (type == "texture_displacement"){num = std::to_string(numDisp++);}
        shader.Activate();
        shader.setHandleui64ARB((type + "_Handle").c_str(), textures[i].handle);
        textures[i].texUnit(shader, (type).c_str(), textures[i].unit);
        textures[i].Bind();
    }
    // Camera Matrix
    shader.Activate();
    glUniform3f(glGetUniformLocation(shader.ID, "camPos"), Camera.Position.x, Camera.Position.y, Camera.Position.z);
    Camera.Matrix(shader, "camMatrix");

    glm::mat4 finalMeshMat = globalMeshMatrix * meshMatrix;

    //glm::mat4 newLMat = FE_Math::composeMatrixWDegrees(position, scale, rotation);
    //glm::mat4 newGMat = FE_Math::composeMatrixWDegrees(globalPosition, globalScale, globalRotation);

	//glm::mat4 newMat = newGMat * newLMat;

    // Push model matrix to the vertex shader
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(finalMeshMat));
    glm::mat3 model3x3 = glm::mat3(finalMeshMat);
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

void Mesh::drawInstanced(Shader& shader, Camera Camera, int instanceCount)
{
    std::cout << "drawing instance" << instanceCount << std::endl;
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



void Mesh::updateMatrix(glm::mat4 matrix)
{
    Mesh::meshMatrix = matrix;
}
void Mesh::updatePosition(glm::vec3 position)
{
    Mesh::position = position;
}
void Mesh::updateRotation(glm::vec3 rotation)
{
    Mesh::rotation = rotation;
}
void Mesh::updateScale(glm::vec3 scale)
{
    Mesh::scale = scale;
}
void Mesh::updateGlobalMatrix(glm::mat4 matrix)
{
	Mesh::globalMeshMatrix = matrix;
}
void Mesh::updateGlobalPosition(glm::vec3 position)
{
    Mesh::globalPosition = position;
}
void Mesh::updateGlobalScale(glm::vec3 scale)
{
    Mesh::globalScale = scale;
}
void Mesh::updateGlobalRotation(glm::vec3 rotation)
{
    Mesh::globalRotation = rotation;
}