#include "ocean.h"
#include <Render/Handler/RenderClass.h>
#include "Render/Handler/RenderHandler.h"
#include  "Systems/util/UUID.h"
#include "utils/FE_math.h"
#include "Render/Handler/CubeVisualizer.h"

void ocean::updatePosition(glm::vec3 Position)
{globalTransformation.position = Position;}

void ocean::updateRotation(glm::vec3 Rotation)
{globalTransformation.rotation = Rotation;}

void ocean::updateScale(glm::vec3 Scale)
{globalTransformation.scale = Scale;}

void ocean::updateTranformation()
{
    gMatrix = FE_Math::composeMatrixWDegrees(globalTransformation.position, globalTransformation.scale, globalTransformation.rotation); // this part compute once
    
    for (int i = 0; i < pMeshes.size(); ++i){
        pMeshes[i].updateGlobalMatrix(gMatrix);
        pMeshes[i].updateGlobalPosition(globalTransformation.position);
        pMeshes[i].updateGlobalScale(globalTransformation.scale);
        pMeshes[i].updateGlobalRotation(globalTransformation.rotation);
    }
    
}

void ocean::updatePrevPosition(glm::vec3 Position)
{prevGlobalTransformation.position = Position;}

void ocean::updatePrevRotation(glm::vec3 Rotation)
{prevGlobalTransformation.rotation = Rotation;}

void ocean::updatePrevScale(glm::vec3 Scale)
{prevGlobalTransformation.scale = Scale;}

void ocean::updatePrevTranformation()
{
    pgMatrix = FE_Math::composeMatrixWDegrees(prevGlobalTransformation.position, prevGlobalTransformation.scale, prevGlobalTransformation.rotation);
    
    for (int i = 0; i < pMeshes.size(); ++i)
    {
        pMeshes[i].updatePrevGlobalMatrix(pgMatrix);
    }
    
}

ocean::ocean(float radius,  float width)
{
    UUID = UUID::returnHandle();
    
    // manual creation (just for now)
    pMeshes.push_back(GenerationTools::generateSubdivededPlaneMesh(128, width));
    pMeshes.push_back(GenerationTools::generateSubdivededPlaneMesh(98, width));
    pMeshes.push_back(GenerationTools::generateSubdivededPlaneMesh(64, width));
    pMeshes.push_back(GenerationTools::generateSubdivededPlaneMesh(32, width));
    pMeshes.push_back(GenerationTools::generateSubdivededPlaneMesh(16, width));
    pMeshes.push_back(GenerationTools::generateSubdivededPlaneMesh(8, width));
    pMeshes.push_back(GenerationTools::generateSubdivededPlaneMesh(4, width));
    pMeshes.push_back(GenerationTools::generateSubdivededPlaneMesh(1, width));
    //pMeshes
    
    generateChunks(radius, width);
    
}

void ocean::assignUUID(uint64_t input)
{
    UUID = input;
}

ocean::~ocean()
{

}

void ocean::generateChunks(int radius, int stepsize)
{
    for (int x = -radius; x < radius; ++x)
    {
        //for (int y = -radius; y < radius; ++y)
        //{
            for (int z = -radius; z < radius; ++z)
            {
                GenerationTools::chunk nOC;
                nOC.position = glm::vec3(x, 0, z)  * glm::vec3(stepsize);
                nOC.areaXZ = stepsize;
                chunks.push_back(nOC);
            }
        //}
    }
}

void ocean::tagChunkLODLevels(float transitionDistance, glm::vec3 vPosition)
{
    GenerationTools::tagChunkLODLevels(transitionDistance, vPosition, maxLOD, chunks);
}

void ocean::draw(Shader shader, Camera Camera)
{
    //tMesh.draw(shader, Camera);
    //for (int i = 0; i < pMeshes.size(); ++i){
    for (int i = 0; i < chunks.size(); ++i)
    {
        glm:: mat4 lmatrix = FE_Math::composeMatrixWDegrees(chunks[i].position, glm::vec3(1.0), glm::vec3(0.0)); // this compute per chunk
    
        pMeshes[chunks[i].LOD_LEVEL].updateMatrix(lmatrix);
        pMeshes[chunks[i].LOD_LEVEL].updatePosition(chunks[i].position);
        pMeshes[chunks[i].LOD_LEVEL].updateRotation(glm::vec3(0.0));
        pMeshes[chunks[i].LOD_LEVEL].updateScale(glm::vec3(1.0));
                
        pMeshes[chunks[i].LOD_LEVEL].draw(shader, Camera);
    }
    
}

void ocean::dbgChunkDraw()
{
    for (int i = 0; i < chunks.size(); ++i){
        CubeVisualizer::draw(chunks[i].position, glm::vec3(chunks[i].areaXZ * 0.5), chunks[i].colourLod,  2.0, true, false);
    }
}