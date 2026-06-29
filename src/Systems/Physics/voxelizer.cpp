#include "voxelizer.h"
#include "utils/FE_math.h"
#include "Systems/Physics/BVH.h"

std::vector<Collision::AABB> voxelizer::voxelizeMeshKD(std::vector<Vertex>& vertices, std::vector<GLuint>& indices,
    Collision::AABB root, int steps, int minTri, glm::vec3 minSize, bool doVertexSnap){
    
    std::vector<Collision::AABB> nAABs;
    
    if (!doVertexSnap){
        voxelizerInternalKD(vertices, indices, root, steps, minTri, minSize, nAABs);
        return nAABs;
    }
    voxelizerInternalKDwVertSnap(vertices, indices, root, steps, minTri, minSize, nAABs);
    return nAABs;
}

std::vector<Collision::AABB> voxelizer::voxelizeMeshKD(std::vector<Vertex>& vertices,
    std::vector<GLuint>& indices, Collision::AABB root, int steps, int minTri,  glm::vec3 minSize, bool doVertexSnap, glm::mat4 transformation){
    
    std::vector<Collision::AABB> nAABs;
    std::vector<Vertex> nVertices = vertices;
    
    // transform
    for (int i = 0; i < nVertices.size(); ++i)
        FE_Math::transformPoint(nVertices[i].position, transformation);
    
    if (!doVertexSnap){
        voxelizerInternalKD(nVertices, indices, root, steps, minTri, minSize, nAABs);
        return nAABs;
    }
    voxelizerInternalKDwVertSnap(vertices, indices, root, steps, minTri, minSize, nAABs);
    return nAABs;
}

bool voxelizer::voxelizerInternalKD(std::vector<Vertex>& vertices, std::vector<GLuint>& indices, Collision::AABB root,
                                    int steps, int minTri, glm::vec3 minSize, std::vector<Collision::AABB>& AABBs){
    
    if (steps <= 0) return false;
    
    int hitCount = 0; // use for triangles hit
    
    std::vector<GLuint> nIndices;
    
    // does collision
    for (int i = 0; i < indices.size(); i += 3){
        
        unsigned int i0 = indices[i];
        unsigned int i1 = indices[i + 1];
        unsigned int i2 = indices[i + 2];
        
        if (i0 >= vertices.size() ||
        i1 >= vertices.size() ||
        i2 >= vertices.size())
            continue;
        
        // transform point can go here, but not yet
        glm::vec3 a = vertices[i0].position;
        glm::vec3 b = vertices[i1].position;
        glm::vec3 c = vertices[i2].position;
		
        // collision sat here
        Collision::HitResult trihit = Collision::SATTriangleVSAABB(a, b, c, root.position, root.size);
        if (trihit.isColliding){
            nIndices.push_back(i0);
            nIndices.push_back(i1);
            nIndices.push_back(i2);
            
            hitCount++;
        }
    }
    
    // if under min or nohit
    if (hitCount < minTri || hitCount <= 0) return false;
    //if (hitCount <= 0) return false;
    
    // should kd split and test both
    Collision::KDsplit kds = Collision::KDsplitVolume(root.position, root.size);
    // if children have any axis below min size snap them, and if all are return false
    // going for if statements so i can filter which axis are min
    int countBelowMinimum = 0;
    if (kds.firstSplit.size.x < minSize.x){ kds.firstSplit.size.x  = minSize.x; countBelowMinimum++;}
    if (kds.firstSplit.size.y < minSize.y){ kds.firstSplit.size.y  = minSize.y; countBelowMinimum++;}
    if (kds.firstSplit.size.z < minSize.z){ kds.firstSplit.size.z  = minSize.z; countBelowMinimum++;}
    if (kds.secondSplit.size.x < minSize.x){ kds.secondSplit.size.x  = minSize.x; countBelowMinimum++;}
    if (kds.secondSplit.size.y < minSize.y){ kds.secondSplit.size.y  = minSize.y; countBelowMinimum++;}
    if (kds.secondSplit.size.z < minSize.z){ kds.secondSplit.size.z  = minSize.z; countBelowMinimum++;}
    
    if (countBelowMinimum >= 6) return false;
    // ^^ yandere dev ahh code
    
    //kds.firstSplit.size
    // traverse down
    bool rA = voxelizerInternalKD(vertices, nIndices, kds.firstSplit, steps - 1, minTri, minSize, AABBs);
    bool rB = voxelizerInternalKD(vertices, nIndices, kds.secondSplit, steps - 1, minTri, minSize, AABBs);
    
    // if both are false, pushback the root
    if (!rA && !rB) AABBs.push_back(root);
    
    return true;
}

bool voxelizer::voxelizerInternalKDwVertSnap(std::vector<Vertex>& vertices, std::vector<GLuint>& indices,
    Collision::AABB root, int steps, int minTri, glm::vec3 minSize, std::vector<Collision::AABB>& AABBs)
{
        
    if (steps <= 0) return false;
    
    int hitCount = 0; // use for triangles hit
    
    std::vector<GLuint> nIndices;
    std::vector<Vertex> nVertices;
    
    // does collision
    for (int i = 0; i < indices.size(); i += 3){
        
        unsigned int i0 = indices[i];
        unsigned int i1 = indices[i + 1];
        unsigned int i2 = indices[i + 2];
        
        if (i0 >= vertices.size() ||
        i1 >= vertices.size() ||
        i2 >= vertices.size())
            continue;
        
        // transform point can go here, but not yet
        glm::vec3 a = vertices[i0].position;
        glm::vec3 b = vertices[i1].position;
        glm::vec3 c = vertices[i2].position;
		
        // collision sat here
        Collision::HitResult trihit = Collision::SATTriangleVSAABB(a, b, c, root.position, root.size);
        if (trihit.isColliding){
            nIndices.push_back(i0);
            nIndices.push_back(i1);
            nIndices.push_back(i2);
            
            nVertices.push_back(vertices[i0]);
            nVertices.push_back(vertices[i1]);
            nVertices.push_back(vertices[i2]);
            
            hitCount++;
        }
    }
    
    // if under min or nohit
    if (hitCount < minTri || hitCount <= 0) return false;
    //if (hitCount <= 0) return false;
    
    Collision::rubiksCubePoints nRubikzCube = Collision::fetchFurthestVertices(nVertices);
    Collision::AABB nAABB = BVH::rootNodeFromRubixPoints(nRubikzCube);
    
    // should kd split and test both
    Collision::KDsplit kds = Collision::KDsplitVolume(nAABB.position, nAABB.size);
    // if children have any axis below min size snap them, and if all are return false
    // going for if statements so i can filter which axis are min
    int countBelowMinimum = 0;
    axisSnap(kds.firstSplit, countBelowMinimum, minSize);
    axisSnap(kds.secondSplit, countBelowMinimum, minSize);
    
    if (countBelowMinimum >= 6) return false;
    // ^^ yandere dev ahh code
    
    // traverse down
    bool rA = voxelizerInternalKDwVertSnap(vertices, nIndices, kds.firstSplit, steps - 1, minTri, minSize, AABBs);
    bool rB = voxelizerInternalKDwVertSnap(vertices, nIndices, kds.secondSplit, steps - 1, minTri, minSize, AABBs);
    
    // if both are false, pushback the root
    if (!rA && !rB) AABBs.push_back(root);
    
    return true;
}

void voxelizer::axisSnap(Collision::AABB& aabb, int& countBelowMinimum, glm::vec3 minSize){
    if (aabb.size.x < minSize.x){ aabb.size.x  = minSize.x; countBelowMinimum++;}
    if (aabb.size.y < minSize.y){aabb.size.y  = minSize.y; countBelowMinimum++;}
    if (aabb.size.z < minSize.z){ aabb.size.z  = minSize.z; countBelowMinimum++;}
}
