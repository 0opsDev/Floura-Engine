#ifndef FLOURA_DEFERRED_CLASS_H
#define FLOURA_DEFERRED_CLASS_H

#include <Render/Shader/shaderClass.h>
#include <glad/gl.h>
#include "Render/Object/RenderQuad.h"

class FlouraDeferred {
public:
    
    static void init();
    
    static void createShaders();
    
    static void draw();
    
    static void Delete();
    
private:
    
public:
    static Shader DFL_Shader;
    static Shader SSR_Shader;
    

    static void DeferredLightingPass();
    static void ssrPass();
    
};

#endif