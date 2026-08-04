#ifndef SWRT_CLASS_H
#define SWRT_CLASS_H

#include <Render/Shader/shaderClass.h>

class FlouraSWRT {
public:

    
    static bool doTemporalAccumulation;
    static bool doDenoise;
    static bool doDenoiseSplitDBGView;
    static int denoiseRadius;
    static float temporalAccumulationBlendFactor;
    
    static bool doHalfRes;
    
    static double rmTime;
    
    
    static void initShaders();
    
    static void cleanupShaders();
    
    static void setupSWRTbuffers(unsigned int width, unsigned int height);
    
    static void updateSWRTbuffersResolution(unsigned int width, unsigned int height);
    
    static void cleanupSWRTbuffers();
    
    static void draw();
    
private:
    static unsigned int swrtBuffer, indirect, specular, emission, indirectSpecular, emissionSpecular, direct, filteredVariance;
    static unsigned int swrtHBuffer, hIndirect, hEmission, hIndirectSpecular, hEmissionSpecular, presentImage, swrtHDepth; 
    
    static Shader raymarchRQShader;
    static Shader raymarchCompShader;
    static Shader denoisePrePassShader;
    static Shader denoiseShader;
    static Shader presentShader;
    
    static void rmPassRenderQuad(unsigned int w, unsigned int h);
    static void rmPassCompute(unsigned int w, unsigned int h);
    static void denoisePrePass();
    static void denoisePass();
    static void presentPass();

};

#endif