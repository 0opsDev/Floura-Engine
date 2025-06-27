#ifndef LIGHTINGPASS_CLASS_H
#define LIGHTINGPASS_CLASS_H

#include<iostream>

#include "camera/Camera.h"
//#include <gl/GL.h>

class LightingPass
{
public:

	static GLuint computeTexture;
	static int samplecount;

	static void init();

	static void initcomputeShader(unsigned int width, unsigned int height);

	static void resizeTexture(unsigned int width, unsigned int height);

	static void computeRender();

};
#endif // LIGHTINGPASS_CLASS_H
