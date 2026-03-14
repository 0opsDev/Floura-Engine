#include "terrain.h"
#include "Core/Render.h"
#include "Render/Handler/RenderHandler.h"
#include  "Systems/util/UUID.h"
#include "utils/FE_math.h"
#include "Render/procedural/GenerationTools.h"

void Terrain::updatePosition(glm::vec3 Position)
{globalTransformation.position = Position;}

void Terrain::updateRotation(glm::vec3 Rotation)
{globalTransformation.rotation = Rotation;}

void Terrain::updateScale(glm::vec3 Scale)
{globalTransformation.scale = Scale;}

void Terrain::updateTranformation()
{
    gMatrix = FE_Math::composeMatrixWDegrees(globalTransformation.position, globalTransformation.scale, globalTransformation.rotation);
    glm:: mat4 lmatrix = FE_Math::composeMatrixWDegrees(glm::vec3(0.0), glm::vec3(1.0), glm::vec3(0.0));

    tMesh.updateMatrix(lmatrix);
    tMesh.updatePosition(glm::vec3(0.0));
    tMesh.updateRotation(glm::vec3(0.0));
    tMesh.updateScale(glm::vec3(1.0));

    tMesh.updateGlobalMatrix(gMatrix);
    tMesh.updateGlobalPosition(globalTransformation.position);
    tMesh.updateGlobalScale(globalTransformation.scale);
    tMesh.updateGlobalRotation(globalTransformation.rotation);
}

void Terrain::updatePrevPosition(glm::vec3 Position)
{prevGlobalTransformation.position = Position;}

void Terrain::updatePrevRotation(glm::vec3 Rotation)
{prevGlobalTransformation.rotation = Rotation;}

void Terrain::updatePrevScale(glm::vec3 Scale)
{prevGlobalTransformation.scale = Scale;}

void Terrain::updatePrevTranformation()
{
    pgMatrix = FE_Math::composeMatrixWDegrees(prevGlobalTransformation.position, prevGlobalTransformation.scale, prevGlobalTransformation.rotation);
    tMesh.updatePrevGlobalMatrix(pgMatrix);
}

Terrain::Terrain(float div,  float width)
{
    UUID = UUID::returnHandle();
    
    tMesh = GenerationTools::generateSubdivededPlaneMesh(div, width);
}

void Terrain::assignUUID(uint64_t input)
{
    UUID = input;
}

Terrain::~Terrain()
{

}

void Terrain::draw(Shader shader, Camera Camera)
{
    tMesh.draw(shader, Camera);
}