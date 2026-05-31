#include "loadHandler.h"

void LoadHandler::updateFromOpenGLThread()
{
    RenderUUID_ModelMeshCreateCall();
    RenderUUID_ModelTextureCreateCall();
}

void LoadHandler::addToModelMeshCreateW_RenderIDQueue(uint64_t UUID)
{
    ModelMeshCreateRenderIDQueue.push_back(UUID);
}

void LoadHandler::addToModelTextureCreateW_RenderIDQueue(uint64_t UUID)
{
    ModelTextureCreateRenderIDQueue.push_back(UUID);
}

std::vector<uint64_t> LoadHandler::ModelMeshCreateRenderIDQueue;
std::vector<uint64_t> LoadHandler::ModelTextureCreateRenderIDQueue;

void LoadHandler::RenderUUID_ModelMeshCreateCall()
{
   //return;
    
    if (ModelMeshCreateRenderIDQueue.empty()) return; // if empty return
    
    for (int i = 0; i < ModelMeshCreateRenderIDQueue.size(); ++i)
    {
        // fetch model index
        int index = RenderHandler::fetchModelIndex(ModelMeshCreateRenderIDQueue[i]);

        for (int j = 0; j < RenderHandler::models[index].model->meshes.size(); ++j)
        {
            // call setup mesh
            if (RenderHandler::models[index].model->meshes[j].suppressSetupMeshCall)
                RenderHandler::models[index].model->meshes[j].setupMesh();
        }
    }
    
    ModelMeshCreateRenderIDQueue.clear();
    
}

void LoadHandler::RenderUUID_ModelTextureCreateCall()
{
    if (ModelTextureCreateRenderIDQueue.empty()) return;
    
    for (int i = 0; i < ModelTextureCreateRenderIDQueue.size(); ++i)
    {
        int index = RenderHandler::fetchModelIndex(ModelTextureCreateRenderIDQueue[i]);
        
        if (RenderHandler::models[index].model->disableInitialTextureUploadToGPUFlag)
        {
            for (int j = 0; j < RenderHandler::models[index].model->meshes.size(); ++j)
            {
                if ( RenderHandler::models[index].model->meshes[j].created)
                {
                    for (int z = 0; z < RenderHandler::models[index].model->meshes[j].textures.size(); ++z)
                    {
                        RenderHandler::models[index].model->meshes[j].textures[z].createFromCapture();
                    }
                }
            }
        }
    }
    
    ModelTextureCreateRenderIDQueue.clear();
}
