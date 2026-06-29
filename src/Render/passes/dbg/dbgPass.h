#ifndef DEBUG_PASS_CLASS_H
#define DEBUG_PASS_CLASS_H

#include<iostream>
#include <Render/Object/ModelAssimp.h>
#include <camera/Camera.h>
class dbgPass
{
public:
    
    static bool overlayDebug;

    static unsigned int dbgBuffer, dbgColour, dbgRBO, hDBO;
    
    static void setupDBGbuffers(unsigned int width, unsigned int height);
    
    static void updateDBGResolution(unsigned int width, unsigned int height);
    
};

#endif 
