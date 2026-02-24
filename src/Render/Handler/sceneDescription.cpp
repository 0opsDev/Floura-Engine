#include "SceneDescription.h"

GLuint SceneDescription::triangleSSBOID;
GLuint SceneDescription::meshSSBOID;
GLuint SceneDescription::quickSSBOID;
GLuint SceneDescription::bvhSSBO;
std::vector<SceneDescription::modelData> SceneDescription::modelArray;

void SceneDescription::generateSceneBuffers() {
    // generate triangle buffer
    glGenBuffers(1, &triangleSSBOID);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, triangleSSBOID);
    // allocate 1024 bytes
    glBufferData(GL_SHADER_STORAGE_BUFFER, 1024, NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, triangleSSBOID); // 6
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // Unbind
    //meshSSBOID
	
    // generate triangle buffer
    glGenBuffers(1, &meshSSBOID);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, meshSSBOID);
    // allocate 1024 bytes
    glBufferData(GL_SHADER_STORAGE_BUFFER, 1024, NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, meshSSBOID); // 7
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // Unbind

    //quickSSBOID
    glGenBuffers(1, &quickSSBOID);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, quickSSBOID);
    // allocate 1024 bytes
    glBufferData(GL_SHADER_STORAGE_BUFFER, 1024, NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 9, quickSSBOID); // 8
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // Unbind

    //bvhSSBO
    glGenBuffers(1, &bvhSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, bvhSSBO);
    // allocate 1024 bytes
    glBufferData(GL_SHADER_STORAGE_BUFFER, 1024, NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 10, bvhSSBO); // 9
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // Unbind
}

void SceneDescription::UpdateModelBuffer()
{
    std::vector<triangle> newTriangleArray;
    std::vector<rayMesh> newMeshArray;

    for (size_t x = 0; x < modelArray.size(); x++)
    {
        for (size_t z = 0; z < modelArray[x].harddata.tris.size(); z++)
        {
            triangle newTriangle;
            newTriangle = modelArray[x].harddata.tris[z];
            newTriangleArray.push_back(newTriangle);
        }
        for (size_t z = 0; z < modelArray[x].harddata.meshes.size(); z++)
        {
            rayMesh newMesh;
            newMesh = modelArray[x].harddata.meshes[z];
            newMeshArray.push_back(newMesh);
        }
    }

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, triangleSSBOID);
    glBufferData(GL_SHADER_STORAGE_BUFFER, newTriangleArray.size() * sizeof(triangle), newTriangleArray.data(), GL_STATIC_DRAW); // gl buffer data wipes whole array
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, triangleSSBOID);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    // meshSSBOID
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, meshSSBOID);
    glBufferData(GL_SHADER_STORAGE_BUFFER, newMeshArray.size() * sizeof(rayMesh), newMeshArray.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, meshSSBOID);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

}

void SceneDescription::updateQuickModelData()
{
    std::vector<quickRayModel> newQuickDataArray;
    std::vector<boxRootNode> newRootNodeArray;

    for (size_t i = 0; i < modelArray.size(); i++)
    {
        newQuickDataArray.push_back(modelArray[i].quickdata.quickModel);
        for (size_t x = 0; x < modelArray[i].harddata.meshes.size(); x++)
        {
            boxRootNode meshBox;
            meshBox.rootPos = modelArray[i].quickdata.meshAABBs[x].rootPos;
            meshBox.rootscale = modelArray[i].quickdata.meshAABBs[x].rootscale;
            meshBox.instanceUUID = modelArray[i].harddata.rayModel.instanceUUID;
            meshBox.padding = 0;

            newRootNodeArray.push_back(meshBox);
        }
    }

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, quickSSBOID);
    glBufferData(GL_SHADER_STORAGE_BUFFER, newQuickDataArray.size() * sizeof(quickRayModel), newQuickDataArray.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 9, quickSSBOID);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    // bvhSSBO
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, bvhSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, newRootNodeArray.size() * sizeof(boxRootNode), newRootNodeArray.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 10, bvhSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

}