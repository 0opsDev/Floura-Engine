#ifndef UNIFORM_MANAGER_H
#define UNIFORM_MANAGER_H

#include<iostream>
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include "Systems/util/UUID.h"
#include <Render/Shader/shaderClass.h>

class UniformManager
{
public:
    enum uniformTypeEnum{
        uBoolean,
        uFloat,
        uVec2,
        uVec3,
        uVec4,
        uInt,
        uInt2,
        uInt3,
        uInt4,
    };
    
    struct uniformOBJ{
        // LOOKUP
        uniformTypeEnum type;
        std::string name;
        uint64_t handle;
        
        // Variables (to hold the data) (maybe i could make this better by only holding index of variable, and storing their data in flat arrays? who cares this is good for now)
        bool uBoolean;
        // floating point
        float uFloat;
        glm::vec2 uVec2;
        glm::vec3 uVec3;
        glm::vec4 uVec4;
        // int
        int uInt;
        glm::ivec2 uInt2;
        glm::ivec3 uInt3;
        glm::ivec4 uInt4;
    };
    
    // uniform array
    static std::vector<uniformOBJ> umUniformOBJS;
    static bool compileOnDirty;
    
    // creates empty uniform
    static uint64_t createUniform(uniformTypeEnum type, std::string name){
        uint64_t nUUID = UUID::returnHandle();
        createUniformW_UUID(type, name, nUUID);
        return nUUID;
    }
    
    static void createUniformW_UUID(uniformTypeEnum type, std::string name, uint64_t UUID); // create empty uniform with uuid
    
    static void deleteUniformW_Index(int uniformIndex);
    
    static void sendUniformToShaderW_Index(int uniformIndex, Shader &Shader);
    
    static int getIndexFromHandle(uint64_t handle);
    
private:
    
    static void dirtyCompileCall();
};
#endif