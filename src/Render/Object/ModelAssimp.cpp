#include "ModelAssimp.h"
#include "Systems/util/UUID.h"
#include  <utils/FE_math.h>
#include <Systems/Physics/BVH.h>
#include <assimp/pbrmaterial.h>
#include <assimp/material.h>

void Model::updatePosition(glm::vec3 Position)
{
    globalTransformation.position = Position;
}

void Model::updateRotation(glm::vec3 Rotation)
{
    globalTransformation.rotation = Rotation;
}

void Model::updateScale(glm::vec3 Scale)
{
    globalTransformation.scale = Scale;
}

void Model::updateTranformation()
{

    gModelMatrix = FE_Math::composeMatrixWDegrees(globalTransformation.position, globalTransformation.scale, globalTransformation.rotation);

    for (unsigned int i = 0; i < meshes.size(); i++)
    {
        meshes[i].updateMatrix(lModelMatrix[i]);
        meshes[i].updatePosition(localTransformation[i].position);
		meshes[i].updateRotation(localTransformation[i].rotation);
		meshes[i].updateScale(localTransformation[i].scale);

		meshes[i].updateGlobalMatrix(gModelMatrix);
        meshes[i].updateGlobalPosition(globalTransformation.position);
        meshes[i].updateGlobalScale(globalTransformation.scale);
        meshes[i].updateGlobalRotation(globalTransformation.rotation);
	}

}

Model::Model(const char* file)
{
    UUID = UUID::returnHandle();
	loadModel(file);
}

Model::~Model() {
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

void Model::draw(Shader& shader, Camera Camera)
{
	// draw all meshes and parse in data
    for (unsigned int i = 0; i < meshes.size(); i++)
    {
        meshes[i].draw(shader, Camera);
    }  
}

void Model::drawInstance(Shader& shader, Camera Camera, int instanceCount)
{
    for (unsigned int i = 0; i < meshes.size(); i++)
    {
        meshes[i].drawInstanced(shader, Camera, instanceCount);
    }
}

void Model::createMeshAABBs()
{
    meshAabbPoints.clear();
    rootnodes.clear();

    meshAabbPoints.reserve(meshes.size());
    rootnodes.reserve(meshes.size());

    for (size_t i = 0; i < meshes.size(); i++)
    {
        Collision::rubiksCubePoints newRubikzCube;
        newRubikzCube = Collision::fetchFurthestVertices(meshes[i].vertices);
        meshAabbPoints.push_back(newRubikzCube);

        rootnodes.emplace_back();
	}
}

void Model::updateMeshAABBs()
{
    for (size_t i = 0; i < meshes.size(); i++)
    {
        Collision::AABB newRootNode;
        glm::mat4 finalMeshMat = gModelMatrix * lModelMatrix[i];

        rootnodes[i] = BVH::rootNodeFromRubixPoints(meshAabbPoints[i], finalMeshMat);
    }
}

void Model::loadModel(std::string path)
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

void Model::processNode(aiNode* node, const aiScene* scene)
{
    // process all the node's meshes (if any)
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        processPositions(node);
        meshes.push_back(processMesh(mesh, scene));
    }
    
    // then do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

void Model::processPositions(aiNode* node)
{
    aiMatrix4x4 localTransform = node->mTransformation;
    aiVector3D position;
    aiVector3D scale;
    aiQuaternion rotation;
    localTransform.Decompose(scale, rotation, position);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position.x, position.y, position.z));
    glm::mat4 rotation_matrix = glm::mat4_cast(glm::quat(rotation.w, rotation.x, rotation.y, rotation.z));
    model = model * rotation_matrix;
    model = glm::scale(model, glm::vec3(scale.x, scale.y, scale.z));
    
    //localTransformation
    transformation newTransformation;
    newTransformation.position = glm::vec3(position.x, position.y, position.z);
    newTransformation.scale = glm::vec3(scale.x, scale.y, scale.z);
    newTransformation.qRotation = glm::quat(rotation.w, rotation.x, rotation.y, rotation.z);
    glm::vec3 euler_radians = glm::eulerAngles(newTransformation.qRotation);
    newTransformation.rotation = glm::degrees(euler_radians);

    //std::cout << "ppAI pos: x: " << position.x << " y: " << position.y << " z: " << position.z << std::endl;
    //std::cout << "PP pos: x: " << newTransformation.position.x << " y: " << newTransformation.position.y << " z: " << newTransformation.position.z << std::endl;
    localTransformation.push_back(newTransformation);
    lModelMatrix.push_back(model);
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
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

        vector.x = mesh->mTangents[i].x;
        vector.y = mesh->mTangents[i].y;
        vector.z = mesh->mTangents[i].z;
        vertex.tangent = vector;

        vector.x = mesh->mBitangents[i].x;
        vector.y = mesh->mBitangents[i].y;
        vector.z = mesh->mBitangents[i].z;
        vertex.biTangent = vector;

        //if (mesh->GetNumColorChannels() > 0)
        //{
            //vertex.color.x = mesh->mColors[1][i].r;
            //vertex.color.y = mesh->mColors[1][i].g;
            //vertex.color.z = mesh->mColors[1][i].b;
        //}
        //else
            vertex.color = glm::vec3(1.0f, 1.0f, 1.0f);

        if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates? // determine colour here
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
    if (mesh->mMaterialIndex >= 0) // needs to check material type like "vec4 col instead of texture"
    {

        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        std::vector<Texture> diffuseMaps = aloadMaterialTextures(material,
            aiTextureType_DIFFUSE, "texture_diffuse", 0);
        std::vector<Texture> roughnessMaps = aloadMaterialTextures(material,
            aiTextureType_DIFFUSE_ROUGHNESS, "texture_roughness", 1); // Note: Could also be aiTextureType_SHININESS
        std::vector<Texture> normalMaps = aloadMaterialTextures(material,
            aiTextureType_NORMALS, "texture_normal", 2);
        //std::vector<Texture> displacementMaps = aloadMaterialTextures(material,
        //    aiTextureType_DISPLACEMENT, "texture_displacement", 3);
        // just gonna go for a alpha on normal (you'll need to manually add that)

        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        textures.insert(textures.end(), roughnessMaps.begin(), roughnessMaps.end());
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        //textures.insert(textures.end(), displacementMaps.begin(), displacementMaps.end());

    }
    aiString name = mesh->mName;
    Mesh nMesh;
	nMesh.name = name.C_Str();
    nMesh.create(vertices, indices, textures);
    return Mesh(nMesh);
}
/*
} else if (material->GetTexture(aiTextureType_HEIGHT, 0, &path) == AI_SUCCESS) {
    // Fallback heightmap found
}

*/
std::vector<Texture> Model::aloadMaterialTextures(aiMaterial* mat, aiTextureType type,
    std::string typeName, int slot)
{
    std::vector<Texture> textures;

    aiTextureType targetType = type;
    //if (type == aiTextureType_DISPLACEMENT && mat->GetTextureCount(type) == 0) {
     //   targetType = aiTextureType_HEIGHT;
    //}


    if (mat->GetTextureCount(targetType) == 0)
    {
        glm::vec4 colourFloat = glm::vec4(0.0f);
        if (type == aiTextureType_DIFFUSE)
        {
            aiColor4D albedoFactor(1.0f, 1.0f, 1.0f, 1.0f);

            mat->Get(AI_MATKEY_COLOR_DIFFUSE, albedoFactor);

            colourFloat = glm::vec4(albedoFactor[0], albedoFactor[1], albedoFactor[2], albedoFactor[3]);
        }
        if (type == aiTextureType_DIFFUSE_ROUGHNESS)
        {
            ai_real metallic = 1.0f;
            ai_real roughness = 1.0f;

            mat->Get(AI_MATKEY_METALLIC_FACTOR, metallic);
            mat->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness);


            colourFloat = glm::vec4(0.0f, roughness, metallic, 1.0f);
        }
        if (type == aiTextureType_NORMALS)
        {
            colourFloat = glm::vec4(0.5f, 0.5f, 1.0f, 1.0f);
        }
        /*
        if (type == aiTextureType_DISPLACEMENT)
        {
            colourFloat = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        }
        if (type == aiTextureType_HEIGHT)
        {
            colourFloat = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        }
        */
        //std::cout << "typeName: " << typeName << std::endl;
        //std::cout << "typenum: " << type << std::endl;
        //std::cout << "colour: " << " r: " << colourFloat.x << " g: " << colourFloat.y << " b: " << colourFloat.z << std::endl;

        Texture texture;
        texture.createColour(colourFloat, (typeName).c_str(), slot);
        textures.push_back(texture);
        loadedTex.push_back(texture);
        loadedTexPath.push_back("null");
    }

    for (unsigned int i = 0; i < mat->GetTextureCount(targetType); i++)
    {
        aiString str; // str is path
        mat->GetTexture(targetType, i, &str);
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

            texture.createTexture(path.c_str(), (typeName).c_str(), slot);
            //std::cout << path << std::endl;
            textures.push_back(texture);
            loadedTex.push_back(texture);
            loadedTexPath.push_back(str.C_Str());
        }   
    }
    return textures;
}