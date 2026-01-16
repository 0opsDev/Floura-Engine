#ifndef SSR_CLASS_H
#define SSR_CLASS_H

#include<iostream>

#include "camera/Camera.h"
//#include <gl/GL.h>

class ssao
{
public:

	static GLuint ssaoTexture;

	static void init(int width, int height);

	static void resize(int width, int height);

	static void draw(int &depth);

private:
	static int currentWidth;
	static int currentHeight;
	static Shader ssaoCompute;
};
#endif // SSR_CLASS_H
