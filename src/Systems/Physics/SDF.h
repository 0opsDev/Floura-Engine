#ifndef SDF_CLASS_H
#define SDF_CLASS_H


#include <Render/Buffer/VBO.h>
#include <Render/Object/texture3D.h>
#include <Systems/Physics/Collision.h>
#include <Systems/Physics/BVH.h>

class flouraSDF
{
public:
    
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