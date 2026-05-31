#ifndef LOAD_HANDLER_CLASS_H
#define LOAD_HANDLER_CLASS_H

#include "Render/Object/Mesh.h"
#include "Render/Handler/RenderHandler.h"
#include <vector>

class LoadHandler
{
public:
    
    static void updateFromOpenGLThread();
    
    static void addToModelMeshCreateW_RenderIDQueue(uint64_t UUID);
    static void addToModelTextureCreateW_RenderIDQueue(uint64_t UUID);

private:
    
    static std::vector<uint64_t> ModelMeshCreateRenderIDQueue;
    static std::vector<uint64_t> ModelTextureCreateRenderIDQueue;
    
    static void RenderUUID_ModelMeshCreateCall();
    static void RenderUUID_ModelTextureCreateCall();
    
};
#endif