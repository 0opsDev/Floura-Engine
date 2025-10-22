#include "ModelAssimp.h"
#include <glm/glm.hpp>                // Core GLM types (like glm::vec3, glm::mat4)
#include <glm/gtc/matrix_transform.hpp> // For glm::scale and glm::translate
#include <glm/gtx/quaternion.hpp>       // For glm::mat4_cast (quaternion to matrix conversion)

void aModel::updatePosition(glm::vec3 Position)
{
    aModel::gPosition = Position;
}

void aModel::updateRotation(glm::vec3 Rotation)
{
    aModel::gRotation = Rotation;
}

void aModel::updateScale(glm::vec3 Scale)
{
    aModel::gScale = Scale;
}

void aModel::create(const char* file)
{
	loadModel(file);
}

void aModel::draw(Shader& shader)
{
    glm::mat4 globalTrans = glm::mat4(1.0f);
    glm::mat4 globalRot = glm::mat4(1.0f);
    glm::mat4 globalSca = glm::mat4(1.0f);

    globalTrans = glm::translate(globalTrans, gPosition);

    globalRot = glm::rotate(globalRot, glm::radians(gRotation.x), glm::vec3(1, 0, 0));
    globalRot = glm::rotate(globalRot, glm::radians(gRotation.y), glm::vec3(0, 1, 0));
    globalRot = glm::rotate(globalRot, glm::radians(gRotation.z), glm::vec3(0, 0, 1));


    globalSca = glm::scale(globalSca, gScale);

    glm::mat4 gModelMatrix = globalTrans * globalRot * globalSca;
	// draw all meshes and parse in data
    for (unsigned int i = 0; i < meshes.size(); i++)
    {
        glm::mat4 finalMatrix = gModelMatrix * lModelMatrix[i]; // * by local transform
        meshes[i].draw(shader, finalMatrix);
    }

        
}

void aModel::Delete() {
    // Delete mesh first then clear all array inside model
    for (size_t i = 0; i < meshes.size(); i++)
    {
        meshes[i].Delete();
        meshes.erase(meshes.begin() + i);
    }
    meshes.clear();

    for (size_t i = 0; i < loadedTexPath.size(); i++)
    {
        loadedTexPath.erase(loadedTexPath.begin() + i);
    }
    loadedTexPath.clear();

    for (size_t i = 0; i < loadedTex.size(); i++)
    {
        loadedTex[i].Delete();
        loadedTex.erase(loadedTex.begin() + i);
    }
    loadedTex.clear();

}

void aModel::loadModel(std::string path)
{
    Assimp::Importer import;
    const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices);
    //const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}

void aModel::processNode(aiNode* node, const aiScene* scene)
{
    // process all the node's meshes (if any)
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
        processPositions(node);

    }
    // then do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

void aModel::processPositions(aiNode* node)
{
    aiMatrix4x4 localTransform = node->mTransformation;
    aiVector3D position;
    aiVector3D scale;
    aiQuaternion rotation;
    localTransform.Decompose(scale, rotation, position);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(scale.x, scale.y, scale.z));
    glm::mat4 rotation_matrix = glm::mat4_cast(glm::quat(rotation.w, rotation.x, rotation.y, rotation.z));
    model = model * rotation_matrix;
    model = glm::translate(model, glm::vec3(position.x, position.y, position.z));

    lPosition.push_back(glm::vec3(position.x, position.y, position.z));
    lScale.push_back(glm::vec3(scale.x, scale.y, scale.z));
    lRotation.push_back(glm::quat(rotation.w, rotation.x, rotation.y, rotation.z));
    lModelMatrix.push_back(model);

}

aMesh aModel::processMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture> textures;
    // verticies
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        // process vertex positions, normals and texture coordinates
        glm::vec3 vector;
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.position = vector;

        vector.x = mesh->mNormals[i].x;
        vector.y = mesh->mNormals[i].y;
        vector.z = mesh->mNormals[i].z;
        vertex.normal = vector;

        if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.texUV = vec;
        }
        else
            vertex.texUV = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }
    // indicies 
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    // textures
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture> diffuseMaps = aloadMaterialTextures(material,
            aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        std::vector<Texture> specularMaps = aloadMaterialTextures(material,
            aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        std::vector<Texture> normalMaps = aloadMaterialTextures(material,
            aiTextureType_NORMALS, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    }

    return aMesh(vertices, indices, textures);
}

std::vector<Texture> aModel::aloadMaterialTextures(aiMaterial* mat, aiTextureType type,
    std::string typeName)
{
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str; // str is path
        mat->GetTexture(type, i, &str);
        bool skip = false;
        for (unsigned int j = 0; j < loadedTex.size(); j++)
        {
            // check if already exsists, if so pushback texture and break
            if (loadedTexPath[j] == (str).C_Str()) // dont have a thing for that yet
            {
                textures.push_back(loadedTex[j]);
                skip = true;
                break;
            }
        }
        if (!skip)
        {
            Texture texture;
            std::string path = directory + "/" + str.C_Str();
            texture.createTexture(path.c_str(), (typeName).c_str(), loadedTex.size());
            std::cout << path << std::endl;
            textures.push_back(texture);
            loadedTex.push_back(texture);
            loadedTexPath.push_back(str.C_Str());
        }
    }
    return textures;
}