#include "SceneDescription.h"
#include <Render/Handler/RenderHandler.h>
#include <Scene/scene.h>
#include <utils/FE_math.h>

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
    glBufferData(GL_SHADER_STORAGE_BUFFER, 1024, NULL, GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, triangleSSBOID); // 6
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // Unbind
    //meshSSBOID
	
    // generate triangle buffer
    glGenBuffers(1, &meshSSBOID);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, meshSSBOID);
    // allocate 1024 bytes
    glBufferData(GL_SHADER_STORAGE_BUFFER, 1024, NULL, GL_STATIC_DRAW);
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

    for (size_t x = 0; x < modelArray.size(); x++){
        for (size_t z = 0; z < modelArray[x].harddata.tris.size(); z++)
            newTriangleArray.push_back(modelArray[x].harddata.tris[z]);
        for (size_t z = 0; z < modelArray[x].harddata.meshes.size(); z++)
            newMeshArray.push_back(modelArray[x].harddata.meshes[z]);
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
    glBufferData(GL_SHADER_STORAGE_BUFFER, newQuickDataArray.size() * sizeof(quickRayModel), newQuickDataArray.data(), GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 9, quickSSBOID);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    // bvhSSBO
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, bvhSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, newRootNodeArray.size() * sizeof(boxRootNode), newRootNodeArray.data(), GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 10, bvhSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

}

// voxel

GLuint SceneDescription::voxelSSBOID;
GLuint SceneDescription::voxelMeshSSBOID;
GLuint SceneDescription::voxelQuickSSBOID;

std::vector<SceneDescription::voxelMesh> SceneDescription::voxelMeshArray;

void SceneDescription::generateVoxelBuffers()
{
    //voxelSSBOID
    glGenBuffers(1, &voxelSSBOID);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, voxelSSBOID);
    // allocate 1024 bytes
    glBufferData(GL_SHADER_STORAGE_BUFFER, 1024, NULL, GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 11, voxelSSBOID); // 8
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // Unbind
    
    //voxelSSBOID
    glGenBuffers(1, &voxelMeshSSBOID);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, voxelMeshSSBOID);
    // allocate 1024 bytes
    glBufferData(GL_SHADER_STORAGE_BUFFER, 1024, NULL, GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 12, voxelMeshSSBOID); // 8
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // Unbind

    //voxelQuickSSBOID
    glGenBuffers(1, &voxelQuickSSBOID);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, voxelQuickSSBOID);
    // allocate 1024 bytes
    glBufferData(GL_SHADER_STORAGE_BUFFER, 1024, NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 13, voxelQuickSSBOID); // 9
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // Unbind
}

void SceneDescription::uploadToVoxelScene(uint64_t instanceUUID)
{
    uint64_t RenderUUID = RenderHandler::findRenderUUIDwIstanceUUID(instanceUUID);
    int index = RenderHandler::fetchModelIndex(RenderUUID);
    if (index != -1){
        
        // prevent instance duplicates
        if (!SceneDescription::voxelMeshArray.empty())
            for (size_t i = 0; i < SceneDescription::voxelMeshArray.size(); i++)
                // id match 
                if (SceneDescription::voxelMeshArray[i].hard.vxUUID == instanceUUID) return;
        
        voxelMeshHard newVoxelMeshHard;
        
        newVoxelMeshHard.vxUUID = instanceUUID;
        
        // build voxels into hard data
        std::vector<voxel> nvxArray;
        std::vector<voxelArray> nvxMeshArray;
        
        int start = 0;
        
        for (int z = 0; z < RenderHandler::models[index].model->VoxelMeshes.size(); ++z) {// everyone of these is a mesh
            voxelArray nvxa;
            nvxa.vxUUID = instanceUUID;
            int nVXcount = 0;
            
            for (int x = 0; x < RenderHandler::models[index].model->VoxelMeshes[z].size(); ++x){ // everyone of these is a voxel
                voxel nVoxel;
                
                Collision::AABB nAABB = RenderHandler::models[index].model->VoxelMeshes[z][x].voxel;
                nVoxel.aabb.instanceUUID = instanceUUID;
                nVoxel.aabb.rootPos = glm::vec4(nAABB.position, 1.0);
                nVoxel.aabb.rootscale = glm::vec4(nAABB.size, 1.0);
                
                nVoxel.albedo = RenderHandler::models[index].model->VoxelMeshes[z][x].material.albedo;
                nVoxel.emission = glm::vec4(RenderHandler::models[index].model->VoxelMeshes[z][x].material.emission, 1.0);
                nVoxel.arm = glm::vec4(RenderHandler::models[index].model->VoxelMeshes[z][x].material.arm, 0.0f);
                
                // build local matrix
                nVoxel.localPosition = glm::vec4(RenderHandler::models[index].model->VoxelMeshes[z][x].voxel.position, 1.0f);
                nVoxel.localScale = glm::vec4(RenderHandler::models[index].model->VoxelMeshes[z][x].voxel.size, 1.0f);
                //nVoxel.localMatrix = glm::inverse(FE_Math::composeMatrixWDegrees(glm::vec3(nVoxel.localPosition), glm::vec3(nVoxel.localScale), glm::vec3(0.0f)));
                
                nvxArray.push_back(nVoxel);
                nVXcount++;
            }
            // store count and start
            nvxa.count = nVXcount;
            nvxa.start = start;
            start += nVXcount;
            
            // push mesh
            nvxMeshArray.push_back(nvxa);
        }
        
        
        newVoxelMeshHard.vxArray = nvxArray;
        newVoxelMeshHard.vxMeshArray = nvxMeshArray;
        
        // push hard into voxel mesh
        voxelMesh nVM;
        nVM.hard = newVoxelMeshHard;
        nVM.quick.vxUUID = instanceUUID;
        voxelMeshArray.push_back(nVM);
        
        // update voxel buffer
        SceneDescription::UpdateVoxelBuffer();
    }
}

void SceneDescription::removeFromVoxelScene(uint64_t instanceUUID){
    for (size_t x = 0; x < SceneDescription::voxelMeshArray.size(); x++)
        if (SceneDescription::voxelMeshArray[x].hard.vxUUID == instanceUUID){
            //std::cout << "Removing model from raytracer with UUID: " << modelUUID << std::endl;
            SceneDescription::voxelMeshArray.erase(SceneDescription::voxelMeshArray.begin() + x);
            break;
        }
    SceneDescription::UpdateVoxelBuffer();
}

void SceneDescription::globalMatrixUpdateVoxel(uint64_t instanceUUID, glm::mat4 newModelMatrix)
{
    for (size_t i = 0; i < SceneDescription::voxelMeshArray.size(); i++)
        if (SceneDescription::voxelMeshArray[i].quick.vxUUID == instanceUUID){
            //std::cout << "bla" << std::endl;
            SceneDescription::voxelMeshArray[i].quick.globalMatrix = glm::inverse(newModelMatrix);
            break;
        }
}

void SceneDescription::UpdateVoxelBuffer()
{
    //std::vector<voxelMeshHard> newVoxelMeshHard;
    
    std::vector<voxel> newVoxelArray;    
    std::vector<voxelArray> newVxMeshArray;

    for (size_t x = 0; x < voxelMeshArray.size(); x++)
    {
        //newVoxelMeshHard.push_back(voxelMeshArray[x].hard);

        // flatten voxels into flat array
        for (int z = 0; z < voxelMeshArray[x].hard.vxArray.size(); ++z)
            newVoxelArray.push_back(voxelMeshArray[x].hard.vxArray[z]);
        
        // flatten meshes into mesh array
        for (int z = 0; z < voxelMeshArray[x].hard.vxMeshArray.size(); ++z)
            newVxMeshArray.push_back(voxelMeshArray[x].hard.vxMeshArray[z]);
    
    }
    
    //glBindBuffer(GL_SHADER_STORAGE_BUFFER, voxelSSBOID);
    //glBufferData(GL_SHADER_STORAGE_BUFFER, newVoxelMeshHard.size() * sizeof(voxelMeshHard), newVoxelMeshHard.data(), GL_STATIC_DRAW);
    //glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 11, voxelSSBOID);
    //glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, voxelSSBOID);
    glBufferData(GL_SHADER_STORAGE_BUFFER, newVoxelArray.size() * sizeof(voxel), newVoxelArray.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 11, voxelSSBOID);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    // 12 goes here
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, voxelMeshSSBOID);
    glBufferData(GL_SHADER_STORAGE_BUFFER, newVxMeshArray.size() * sizeof(voxelArray), newVxMeshArray.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 12, voxelMeshSSBOID);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void SceneDescription::updateQuickVoxelData()
{
    std::vector<voxelMeshQuickData> newVoxelMeshQuickData;

    for (size_t x = 0; x < voxelMeshArray.size(); x++)
        newVoxelMeshQuickData.push_back(voxelMeshArray[x].quick);
    
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, voxelQuickSSBOID);
    glBufferData(GL_SHADER_STORAGE_BUFFER, newVoxelMeshQuickData.size() * sizeof(voxelMeshQuickData), newVoxelMeshQuickData.data(), GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 13, voxelQuickSSBOID);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}
