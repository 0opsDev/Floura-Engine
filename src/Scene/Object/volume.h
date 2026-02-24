#ifndef VOLUME_H
#define VOLUME_H

#include "glm/glm.hpp"
#include "vector"
#include  "string"
class FE_Volume
{
public:
    
    // single should have option to load from cubemap (should make custom format better)
    enum VOL_TYPE
    {
        VOXEL = 0,
        REFLECTION_PROBE_GRID = 1,
        REFLECTION_PROBE_SINGLE = 2,
        LIGHT_PROBE = 3,
        FOG_VOLUME = 4
    };
    
    FE_Volume(VOL_TYPE type);
    ~FE_Volume();
    
    void inheritUUID(uint64_t inputUUID); // replaces generated UUID with inherited one
    
    bool doDebugDraw = false;
    void debugDraw();
    
    uint64_t ID;
    VOL_TYPE volumeType;
    std::string name;
    
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
    
};

#endif