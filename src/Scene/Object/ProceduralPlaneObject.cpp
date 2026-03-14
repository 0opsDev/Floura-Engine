#include "ProceduralPlaneObject.h"

ProceduralPlaneObject::ProceduralPlaneObject(PROCEDURAL_PLANE_OBJECT_TYPE type)
{
    this->type = type;
    
    switch (type)
    {
    case P_TERRAIN_PLANE: 
        
        break;
    case P_OCEAN_PLANE: 
        
        break;
    }
    
}

ProceduralPlaneObject::~ProceduralPlaneObject()
{
    
}
