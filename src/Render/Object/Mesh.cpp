#include "Mesh.h"
#include <camera/Camera.h>
#include <limits>
#include <utils/FE_math.h>

void Mesh::create(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, std::vector<Texture>& textures)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    setupMesh();
}

void Mesh::draw(Shader& shader, Camera camera, glm::mat4 meshMatrix)
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
    glUniform3f(glGetUniformLocation(shader.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
    camera.Matrix(shader, "camMatrix");

    // Push model matrix to the vertex shader
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(meshMatrix));
    glm::mat3 model3x3 = glm::mat3(meshMatrix);
    glm::mat3 normalMatrix = glm::transpose(glm::inverse(model3x3));
    glUniformMatrix3fv(glGetUniformLocation(shader.ID, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
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
