#ifndef VOXELIZER_CLASS_H
#define VOXELIZER_CLASS_H


#include <Render/Buffer/VBO.h>
#include <Render/Object/texture3D.h>
#include <Systems/Physics/Collision.h>

class voxelizer
{
public:

    struct voxelMaterial{
        glm::vec4 albedo;
        glm::vec3 arm; 
        glm::vec3 emission;
    };

    struct voxelObj{
        Collision::AABB voxel;
        voxelMaterial material;
    };
    
    static std::vector<Collision::AABB> voxelizeMeshKD(std::vector<Vertex> &vertices, std::vector<GLuint> &indices, Collision::AABB root, int steps, int minTri, glm::vec3 minSize, bool doVertexSnap);
    static std::vector<Collision::AABB> voxelizeMeshKD(std::vector<Vertex> &vertices, std::vector<GLuint> &indices, Collision::AABB root, int steps, int minTri, glm::vec3 minSize, bool doVertexSnap, glm::mat4 transformation);
    
    static std::vector<Collision::voxelAccel> voxelizeMeshKDAccel(std::vector<Vertex> &vertices, std::vector<GLuint> &indices, Collision::AABB root, int steps, int minTri, glm::vec3 minSize, glm::mat4 transformation);
    
private:
    
    static bool voxelizerInternalKD(std::vector<Vertex> &vertices, std::vector<GLuint> &indices, Collision::AABB root, int steps, int minTri, glm::vec3 minSize, std::vector<Collision::AABB>& AABBs); // needs some sorta AABB array input, mesh geometry
    static bool voxelizerInternalKDwVertSnap(std::vector<Vertex> &vertices, std::vector<GLuint> &indices, Collision::AABB root, int steps, int minTri, glm::vec3 minSize, std::vector<Collision::AABB>& AABBs); // needs some sorta AABB array input, mesh geometry
        
    static bool voxelizerInternalKDAccel(std::vector<Vertex> &vertices, std::vector<GLuint> &indices, Collision::AABB root, int steps, int minTri, glm::vec3 minSize, std::vector<Collision::voxelAccel>& AABBs); // needs some sorta AABB array input, mesh geometry
    
    
    static void axisSnap(Collision::AABB& aabb, int &countBelowMinimum, glm::vec3 minSize);

};

#endif