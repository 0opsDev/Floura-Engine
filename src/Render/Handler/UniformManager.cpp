#include "UniformManager.h"

#include "Core/Render.h"

std::vector<UniformManager::uniformOBJ> UniformManager::umUniformOBJS;
bool UniformManager::compileOnDirty = true;

void UniformManager::createUniformW_UUID(uniformTypeEnum type, std::string name, uint64_t UUID)
{
    uniformOBJ nUniform;
    nUniform.name = name;
    nUniform.type = type;
    nUniform.handle = UUID;
    
    umUniformOBJS.push_back(nUniform);
    
    UniformManager::dirtyCompileCall();
}

void UniformManager::deleteUniformW_Index(int uniformIndex)
{
    umUniformOBJS.erase(umUniformOBJS.begin() + uniformIndex);
    UniformManager::dirtyCompileCall();
}


/*
   uBoolean,
        uFloat,
        uVec2,
        uVec3,
        uVec4,
        uInt,
        uInt2,
        uInt3,
        uInt4,
*/
void UniformManager::sendUniformToShaderW_Index(int uniformIndex, Shader& Shader)
{
    Shader.Activate();
    uniformTypeEnum tType = umUniformOBJS[uniformIndex].type;
    std::string tName = umUniformOBJS[uniformIndex].name;
    
    
    switch (tType)
    {
        case uBoolean:{ Shader.setBool(tName.c_str(), umUniformOBJS[uniformIndex].uBoolean); break; }
        // floating point
        case uFloat:{ Shader.setFloat(tName.c_str(), umUniformOBJS[uniformIndex].uFloat); break; }
        case uVec2:{Shader.setFloat2(tName.c_str(), umUniformOBJS[uniformIndex].uVec2); break;}
        case uVec3:{Shader.setFloat3(tName.c_str(), umUniformOBJS[uniformIndex].uVec3); break;}
        case uVec4:{Shader.setFloat4(tName.c_str(), umUniformOBJS[uniformIndex].uVec4);break; }
        // int
        case uInt:{Shader.setInt(tName.c_str(), umUniformOBJS[uniformIndex].uInt); break;}
        case uInt2:{ Shader.setInt2(tName.c_str(),umUniformOBJS[uniformIndex].uInt2.x, umUniformOBJS[uniformIndex].uInt2.y); break; }
        case uInt3:{Shader.setInt3(tName.c_str(), umUniformOBJS[uniformIndex].uInt3.x, umUniformOBJS[uniformIndex].uInt3.y,umUniformOBJS[uniformIndex].uInt3.z); break;}
        case uInt4:{Shader.setInt4(tName.c_str(), umUniformOBJS[uniformIndex].uInt4.x, umUniformOBJS[uniformIndex].uInt4.y, umUniformOBJS[uniformIndex].uInt4.z, umUniformOBJS[uniformIndex].uInt4.w); break;}
    }
}

int UniformManager::getIndexFromHandle(uint64_t handle)
{
    for (int i = 0; i < umUniformOBJS.size(); i++) if (handle == umUniformOBJS[i].handle) return i;
    return -1;
}

void UniformManager::dirtyCompileCall()
{
    if (!compileOnDirty) return;
    
    RenderClass::compileShaders();
}
