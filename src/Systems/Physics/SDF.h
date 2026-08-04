#ifndef SDF_CLASS_H
#define SDF_CLASS_H


#include <Render/Buffer/VBO.h>
#include <Render/Object/texture3D.h>
#include <Systems/Physics/Collision.h>
#include <Systems/Physics/BVH.h>

class flouraSDF
{
public:
    
    static  int localPerModelMeshCountCap;
    
    struct localSDF{
        glm::vec4 position; // uv.x
        glm::vec4 extents; // uv.y
        glm::vec4 rootPosition;
        glm::vec4 rootExtents;
        glm::vec4 gPosition;
        glm::vec4 gExtents;
        //glm::vec4 gRotation;
        glm::mat4 globalTransform;
        
        uint64_t instanceUUID;
        uint64_t SDF_Handle;
        
        uint64_t texture_diffuse_Handle;
        uint64_t texture_roughness_Handle;
        uint64_t texture_normal_Handle;
        uint64_t texture_emission_Handle;
    };
    
    static GLuint localSDF_SSBOID;
    
    static std::vector<localSDF> localSDFS;
    
    static void uploadToLSDFScene(uint64_t instanceUUID);
    static void removeFromLSDFScene(uint64_t instanceUUID);
    
    static void updateUVscale(uint64_t instanceUUID, glm::vec2& scale);
    static void updateGlobalTransformation(uint64_t instanceUUID, glm::mat4& gt, glm::vec3 gRotation);
    static void updateSDFBuffer();
    static void wipeScene();
    
    static void initLocalScene();
    static void cleanupLocalScene();
    
    static void cacheSDF(const char* path, int hash, std::vector<Texture3D *>& meshSDFs);
    
    // SDF
    static void bakeMeshSDF(std::vector<Vertex> &vertices, std::vector<GLuint> &indices, Collision::AABB root, const int sliceSize, Texture3D& texture, GLuint slot);
    static void bakeMeshSDFAccel(std::vector<Vertex> &vertices, std::vector<Collision::voxelAccel>& voxelAccel, Collision::AABB root, const int sliceSize, Texture3D& texture, GLuint slot);
    static void bakeMeshSDFAccel(std::vector<Vertex> &vertices, std::vector<BVH::BVH_primitive>& prims, Collision::AABB root, const int sliceSize, Texture3D& texture, GLuint slot);
    static void bakeMeshSDFAccel(std::vector<Vertex> &vertices, std::vector<BVH::leaf>& leaves, Collision::AABB root, const int sliceSize, Texture3D& texture, GLuint slot);
    
    static void nearestPointBlasTraversal(std::vector<BVH::leaf>& leaves, std::vector<Vertex>& vertices, float &minDist,int &minIndex, int& closestPrimIndex, int cLeafIndex, glm::vec3& P);
    static void nearestNeighbourPrims(std::vector<BVH::BVH_primitive> &prims, int &closestPrimIndex, glm::vec3& P);
    static void nearestNeighbourPrims(std::vector<Vertex> &vertices, std::vector<BVH::BVH_primitive> &prims, int &closestPrimIndex, glm::vec3& P);
    // cant use this yet until the recursive fucnction is done
    // had enough of collison class so this does 
    static glm::vec3 distanceToClosestPointOnMeshSDFAccel_PlusUV(std::vector<Vertex> &vertices, std::vector<BVH::leaf>& leaves, glm::vec3& P);  // KD
    static glm::vec3 distanceToClosestPointOnMeshSDFAccel_PlusUV(std::vector<Vertex> &vertices, std::vector<BVH::BVH_primitive>& prims, glm::vec3& P);  // BVH PRIM
    
private:

};

#endif