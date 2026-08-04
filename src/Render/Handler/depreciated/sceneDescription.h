#ifndef SCENE_DESCRIPTION_H
#define SCENE_DESCRIPTION_H
#include<iostream>
#include "Render/Object/Texture.h"
//#include "camera/Camera.h"
#include <Render/Object/ModelAssimp.h>

class SceneDescription
{
public:

    struct triangle
    {
        // vec4 for padding
        glm::vec4 a, b, c;
        // texUV
        glm::vec4 aTex;
        glm::vec4 bTex;
        glm::vec4 cTex;
        // normal
        glm::vec4 aNormal, bNormal, cNormal;
        // tangents
        glm::vec4 aTangent, bTangent, cTangent;
        // bitangents
        glm::vec4 aBiTangent, bBiTangent, cBiTangent;
    };
    
    struct rayMesh // diffuse, specular, normal
    {
        int triangleCount; // 4
        int meshIndex; // 4
        int modelIndex;
        int mateialIndex;

        uint64_t instanceUUID;
        uint64_t meshUUID;
        // textures
        uint64_t albedoHandle;
        uint64_t specularHandle;
        uint64_t normalHandle;
        uint64_t emissionHandle;
    };
    
    struct rayModel
    {
        uint64_t instanceUUID;
        int ModelIndex;
        int meshCount;
    };
    
    struct modelHarddata
    {
        rayModel rayModel; // contains the UUID
        std::vector<triangle> tris;
        std::vector<rayMesh> meshes;

        // tracking infomation
        int globalMeshOffset;
        int globalTriangleOffset;
    };
    
    struct quickRayModel
    {
        uint64_t instanceUUID;
        uint64_t padding;
        glm::mat4 ModelMatrix;
        glm::mat4 NormalMatrix;
        glm::vec4 uvScale;
        // would be nice to have an active toggle for if do render is toggled
    };
    
    struct boxRootNode
    {
        glm::vec4 rootPos; // 16
        glm::vec4 rootscale; // 16

        uint64_t instanceUUID;
        uint64_t padding;
    };
    
    struct modelQuickData // for polling updates
    {
        quickRayModel quickModel; // quick model data
        std::vector<boxRootNode> meshAABBs;
    };
    
    struct modelData {// upate variables
        modelHarddata harddata; // full data
        modelQuickData quickdata; // quick update data
    };
    
    static std::vector<modelData> modelArray;
    
    static void generateSceneBuffers();
    
    static void UpdateModelBuffer();
    static void updateQuickModelData();
    
//private:
    static GLuint triangleSSBOID;
    static GLuint meshSSBOID;
    static GLuint quickSSBOID;
    static GLuint bvhSSBO;
    
    /*
    // voxel
    static GLuint voxelSSBOID; // unused rn
    static GLuint voxelMeshSSBOID; // unused rn
    static GLuint voxelQuickSSBOID; // unused rn
    
    struct voxel{
        boxRootNode aabb;
        glm::vec4 albedo; // 16
        glm::vec4 arm; // 16
        glm::vec4 emission; // 16
        //glm::vec4 padding; // 16
        glm::vec4 localPosition; // 16
        glm::vec4 localScale; // 16
        //glm::mat4 localMatrix; // 64
    };
    
    struct voxelArray{
        // start and count
        int start; // 4
        int count; // 4
        int pad1; // 4
        int pad2; // 4
        
        uint64_t vxUUID; // 8
        uint64_t padding; // 8
    };

    struct  voxelMeshHard{
        std::vector<voxel> vxArray;
        std::vector<voxelArray> vxMeshArray;
        
        uint64_t vxUUID;
        uint64_t padding;
    };

    struct voxelMeshQuickData{
        glm::mat4 globalMatrix; 
        uint64_t vxUUID;
        uint64_t padding;
        glm::vec4 padding2;
    };
    
    // for the cpu
    struct voxelMesh{
        voxelMeshHard hard;
        voxelMeshQuickData quick;
    };
    
    static std::vector<voxelMesh> voxelMeshArray;
    
    static void generateVoxelBuffers();
    
    // upload and remove voxel
    static void uploadToVoxelScene(uint64_t instanceUUID);
    static void removeFromVoxelScene(uint64_t instanceUUID);
    
    static void globalMatrixUpdateVoxel(uint64_t instanceUUID, glm::mat4 newModelMatrix);
    
    // upload buffers
    static void UpdateVoxelBuffer();
    static void updateQuickVoxelData();
    */
    
};

#endif
