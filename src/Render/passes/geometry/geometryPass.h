#ifndef GEOMETRYPASS_CLASS_H
#define GEOMETRYPASS_CLASS_H

#include<iostream>
#include <Render/Object/ModelAssimp.h>
#include <camera/Camera.h>
class GeometryPass
{
public:
	// buffer #1
	static unsigned int depthTexture, gBuffer, gAlbedoSpec, gNormal, gSpecular, gPosition, DBO, gNoise, gVelocity, gEmission;
	// buffer #2
	static unsigned int mBuffer, mAlphaMask, mDepth; // maskbuffer
	
	static void updateGbufferResolution(unsigned int width, unsigned int height);

	static void setupGbuffers(unsigned int width, unsigned int height);

	static void gPassDraw(Model*& model, Shader& GPass, Camera camera);
};

#endif // GEOMETRYPASS_CLASS_H
