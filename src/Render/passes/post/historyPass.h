#ifndef HISTORYPASS_CLASS_H
#define HISTORYPASS_CLASS_H

#include<iostream>
#include <Render/Shader/shaderClass.h>
#include "Render/Object/RenderQuad.h"
#include <camera/Camera.h>

class HistoryPass
{
public:
    static unsigned int hDepthTexture, hBuffer, hColour, hNormal, hDBO;
   static Shader hPassShader;
    
    static void init();

    static void updateHbufferResolution(unsigned int width, unsigned int height);

    static void setupHbuffers(unsigned int width, unsigned int height);

    static void hPassDraw();
    
private:

    static RenderQuad hpRenderQuad;

    
};

#endif // HISTORYPASS_CLASS_H
