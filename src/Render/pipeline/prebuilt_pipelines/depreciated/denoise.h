#ifndef DENOISE_CLASS_H
#define DENOISE_CLASS_H

#include<iostream>

#include "camera/Camera.h"
//#include <gl/GL.h>

class denoiser
{
public:

	static GLuint denoiseTexture;
	static bool doDenoise;
	static int minRadius;

	static void init();

	static void initcomputeShader(unsigned int width, unsigned int height);

	static void resizeTexture(unsigned int width, unsigned int height);

	static void render();

private:

	static unsigned int CurrentWidth;
	static unsigned int CurrentHeight;

	static Shader denoiserQuadShader;
	static Shader denoiseCompute;
	static void RenderToQuad();


};
#endif // LIGHTINGPASS_CLASS_H
