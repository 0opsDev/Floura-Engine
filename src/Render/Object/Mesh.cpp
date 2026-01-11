#include "Mesh.h"
#include <utils/logConsole.h>
#include <camera/Camera.h>
#include <limits>
#include <utils/FE_math.h>
#include <Render/Object/SkyBox.h>
#include "Scene/scene.h"
#include "Systems/util/UUID.h"

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

void Mesh::draw(Shader& shader)
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
    glUniform3f(glGetUniformLocation(shader.ID, "camPos"), Scene::maincamera.Position.x, Scene::maincamera.Position.y, Scene::maincamera.Position.z);
    Scene::maincamera.Matrix(shader, "camMatrix");

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

// instead of always scanning thru all the verticies grab the 8 furthest points on each axis and make a box from that
// store these 8 points, then just make a update function that just updates the aabb local transforms, including rotation
Collision::AABB Mesh::createAABBfromMesh() // local and global transfoms needto be applied
{
    // optimise later btw
    Collision::AABB AABB;

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

// placeholders
void Mesh::createAABB()
{
    aabbPoints = Collision::fetchFurthestVertices(this->vertices);

    // create aabb from those points
	boxCollider = Collision::createAABBfromRubiksCubePoints(aabbPoints);
    boxCollider.size = FE_Math::pad(boxCollider.position, 0.1f);
    
}

void Mesh::updateAABB() // no args for now
{
    Collision::rubiksCubePoints newpoints = aabbPoints;


    //glm::mat4 newLMat = FE_Math::composeMatrixWDegrees(position, scale, rotation);
    //glm::mat4 newGMat = FE_Math::composeMatrixWDegrees(globalPosition, globalScale, globalRotation);

    //glm::mat4 newMat = newGMat * newLMat;

	glm::mat4 finalMeshMat = globalMeshMatrix * meshMatrix;
	// up
    FE_Math::transformPoint(newpoints.ULF, finalMeshMat);
    FE_Math::transformPoint(newpoints.URF, finalMeshMat);
    FE_Math::transformPoint(newpoints.URB, finalMeshMat);
    FE_Math::transformPoint(newpoints.ULB, finalMeshMat);
	// down
    FE_Math::transformPoint(newpoints.DLF, finalMeshMat);
    FE_Math::transformPoint(newpoints.DRF, finalMeshMat);
    FE_Math::transformPoint(newpoints.DRB, finalMeshMat);
    FE_Math::transformPoint(newpoints.DLB, finalMeshMat);

    boxCollider = Collision::createAABBfromRubiksCubePoints(newpoints);
	//boxCollider.position += finalGlobalPos + finalpos;
    boxCollider.size = FE_Math::pad(boxCollider.size, 0.1f);
}