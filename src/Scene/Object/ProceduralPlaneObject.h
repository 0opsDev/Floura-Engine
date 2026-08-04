#ifndef PROCEDURAL_PLANE_OBJECT_CLASS_H
#define PROCEDURAL_PLANE_OBJECT_CLASS_H

#include "Render/procedural/ocean.h"
#include "Render/procedural/terrain.h"
#include <Render/pipeline/prebuilt_pipelines/depreciated/raytracer.h>
#include "Scene/ProbeHandler.h"

class ProceduralPlaneObject {
public:
    
    
    enum PROCEDURAL_PLANE_OBJECT_TYPE
    {
        P_TERRAIN_PLANE = 0,
        P_OCEAN_PLANE = 1,
    };
    
    PROCEDURAL_PLANE_OBJECT_TYPE type;
    std::string name;
    uint64_t UUID;
    std::string UUIDstring;
    
    ProceduralPlaneObject(PROCEDURAL_PLANE_OBJECT_TYPE type);
    ~ProceduralPlaneObject();
    
    struct transformation {
        glm::vec3 position = glm::vec3(0.0f);
        glm::vec3 rotation = glm::vec3(0.0f);
        glm::vec3 scale = glm::vec3(1.0f);
    };
    
    struct object
    {
        Terrain* terrain;
        ocean* ocean;
        bool dirtyTransform;
    };
    
    struct component
    {
        transformation transform;
        object object;
        
    };
    
    component component;
    
    // transformations
    glm::vec3 fetchPosition() {return component.transform.position;}
    glm::vec3 fetchRotation() {return component.transform.rotation;}
    glm::vec3 fetchScale() {return component.transform.scale;}
    
    void setPosition(const glm::vec3& position) {
        if (position == component.transform.position) return;
        component.transform.position = position;
        raytracer::RTGlobalTransformFlag = true;
        component.object.dirtyTransform = true;
        ProbeHandler::dirtyScene = true;
    }
    void setRotation(const glm::vec3& rotation) {
        if (rotation == component.transform.rotation) return;
        glm::vec3 nr = rotation;
        if (nr.x > 360) nr.x = 0.0f;
        if (nr.y > 360) nr.y = 0.0f;
        if (nr.z > 360) nr.z = 0.0f;
        component.transform.rotation = nr;
        raytracer::RTGlobalTransformFlag = true;
        component.object.dirtyTransform = true;
        ProbeHandler::dirtyScene = true;
    }
    void setScale(const glm::vec3& scale) {
        if (scale == component.transform.scale) return;
        component.transform.scale = scale;
        raytracer::RTGlobalTransformFlag = true;
        component.object.dirtyTransform = true;
        ProbeHandler::dirtyScene = true;
    }
    
};

#endif
