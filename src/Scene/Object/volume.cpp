#include  "volume.h"
#include "Systems/util/UUID.h"
#include "Render/Cube/CubeVisualizer.h"
#include "Core/Render.h"
#include "Render/Handler/RenderHandler.h"

FE_Volume::FE_Volume(VOL_TYPE type)
{
    volumeType = type;
    ID = UUID::returnHandle();
}

FE_Volume::~FE_Volume()
{
    
}

void FE_Volume::inheritUUID(uint64_t inputUUID)
{
    FE_Volume::ID = inputUUID;
}

void FE_Volume::debugDraw()
{
    if (doDebugDraw) 
        RenderClass::WhiteCube->draw(FE_Volume::position, FE_Volume::scale, glm::vec3(0.0f,0.0f,1.0f), 2.0f, false, false);
}
