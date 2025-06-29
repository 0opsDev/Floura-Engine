#ifndef MODEL_CLASS_H
#define MODEL_CLASS_H

#include <json/json.hpp>
#include "Mesh.h"

using json = nlohmann::json;

class Model {
public:
    Model(const char* file);

    void Draw(Shader& shader, glm::vec3 translation, glm::vec4 rotation, glm::vec3 scale);
    glm::vec3 lastCollisionPoint;
    glm::vec3 lastCollisionFace[3]; // Stores the last triangle touched
    bool checkCollide(glm::vec3 point, glm::vec3 globalTranslation, glm::quat globalRotation, glm::vec3 globalScale, float checkRadius);
    std::vector<Mesh> meshes;
    std::vector<Texture> textures2;
    std::vector<glm::vec3> translationsMeshes;
    std::vector<glm::quat> rotationsMeshes;
    std::vector<glm::vec3> scalesMeshes;
    std::vector<glm::mat4> matricesMeshes;

    int getVertexCount() const;
    int getIndiceCount() const;

private:
    const char* file;
    std::vector<unsigned char> data;
    json JSON;


    std::vector<std::string> loadedTexName;
    std::vector<Texture> loadedTex;

    void loadMesh(unsigned int indMesh);

    // Location of each node
    void traverseNode(unsigned int nextNode, glm::mat4 matrix = glm::mat4(1.0f));

    std::vector<unsigned char> getData();
    std::vector<float> getFloats(json accessor);
    std::vector<GLuint> getIndices(json accessor);
    std::vector<Texture> getTextures();
    std::vector<Texture> getTexturesForMesh(unsigned int indMesh);

    std::vector<Vertex> assembleVertices(
        std::vector<glm::vec3> positions,
        std::vector<glm::vec3> normals,
        std::vector<glm::vec2> texUVs
    );

    std::vector<glm::vec2> groupFloatsVec2(std::vector<float> floatVec);
    std::vector<glm::vec3> groupFloatsVec3(std::vector<float> floatVec);
    std::vector<glm::vec4> groupFloatsVec4(std::vector<float> floatVec);
};

#endif
