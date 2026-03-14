#ifndef UTILITY_PASS_CLASS_H
#define UTILITY_PASS_CLASS_H

#include<iostream>
#include <Render/Shader/shaderClass.h>
#include "Render/Object/RenderQuad.h"
#include <camera/Camera.h>

class UtilityPass
{
public:

    static void init();
    
    static void copy(unsigned int *to, unsigned int *from);
    
private:

    //static RenderQuad upRenderQuad;

    
};

#endif // HISTORYPASS_CLASS_H
