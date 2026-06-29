#include "BVH.h"
#include "utils/FE_math.h"
#include <Render/Handler/RenderHandler.h>


std::vector<BVH::leaf> BVH::meshBlasGen(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, Collision::AABB root, int steps){
    std::vector<BVH::leaf> nBlas;
    
    //Collision::AABB tAABB;
    //if (!Collision::meshAABBCheck(vertices,  indices, root))return nBlas;
    
    //leaf topLeaf;
    //topLeaf.job = 0; // 0 = top
    //topLeaf.aabb = root;
    //Collision::KDsplit split = Collision::KDsplitVolume(topLeaf.aabb.position, topLeaf.aabb.size);
    
    treeInternalClass(vertices, indices, root, steps, -1, nBlas); // -1 is top
    
    return nBlas;
}

Collision::AABB BVH::rootNodeFromRubixPoints(Collision::rubiksCubePoints points,
                                             glm::mat4 ModelMatrix)
{
    Collision::AABB newNode;
    //glm::mat4 newLMat = FE_Math::composeMatrixWDegrees(position, scale, rotation);
    //glm::mat4 newGMat = FE_Math::composeMatrixWDegrees(globalPosition, globalScale, globalRotation);

    //glm::mat4 newMat = newGMat * newLMat;

    Collision::rubiksCubePoints newpoints = Collision::transformRubiks(points, ModelMatrix);

    newNode = Collision::createAABBfromRubiksCubePoints(newpoints);
    newNode.size = FE_Math::pad(newNode.size, 0.1f);
    //boxCollider.position += finalGlobalPos + finalpos;

    return newNode;
}

Collision::AABB BVH::rootNodeFromRubixPoints(Collision::rubiksCubePoints points)
{
    Collision::AABB newNode;

    newNode = Collision::createAABBfromRubiksCubePoints(points);
    newNode.size = FE_Math::pad(newNode.size, 0.1f);
    //boxCollider.position += finalGlobalPos + finalpos;

    return newNode;
}

void BVH::treeInternalClass(std::vector<Vertex>& vertices, std::vector<GLuint>& indices,
                            Collision::AABB root, int steps, int parentIndex, std::vector<leaf>& iblas){
    if (!Collision::meshAABBCheck(vertices,  indices, root)) return;

}
