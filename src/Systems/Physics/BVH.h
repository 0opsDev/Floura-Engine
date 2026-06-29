#ifndef BVH_H
#define BVH_H
#include "glm/glm.hpp"
#include <vector>
#include <Systems/Physics/Collision.h>
#include <Render/Buffer/VBO.h>

class BVH
{
public:
    struct  leaf
    {
        // job and bottoms child information
        int job = 0; // 0 = top, 1 = internal, 2 = bottom
        int startIndex = 0; // for job 2 (bottoms children (example: triangles))
        int count = 0; // for job 2 again, the offset of triangles from start
        
        // relationships
        int parentIndex = 0; // for job 1 and 2, parent that holds them
        // children (if 0 or 1)
        int firstChildIndex = -1; 
        int secondChildIndex = -1; // -1 means it doesnt have children
        
        // leafs bounds
        Collision::AABB aabb;
    };
    
    std::vector<BVH::leaf> meshBlasGen(std::vector<Vertex> &vertices, std::vector<GLuint> &indices, Collision::AABB root, int steps);
    
    static Collision::AABB rootNodeFromRubixPoints(Collision::rubiksCubePoints points,
    glm::mat4 ModelMatrix); // returns rootnode
    
    static Collision::AABB rootNodeFromRubixPoints(Collision::rubiksCubePoints points); // returns rootnode
    
private:
    
    void treeInternalClass(std::vector<Vertex> &vertices, std::vector<GLuint> &indices, Collision::AABB root, int steps, int parentIndex, std::vector<leaf>& iblas); 

};

#endif