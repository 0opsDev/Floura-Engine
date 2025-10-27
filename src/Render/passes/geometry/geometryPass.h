#ifndef GEOMETRYPASS_CLASS_H
#define GEOMETRYPASS_CLASS_H

#include<iostream>
#include <Render/Model/ModelAssimp.h>

class GeometryPass
{
public:
	static unsigned int depthTexture, gBuffer, gAlbedoSpec, gNormal, gPosition, DBO, gNoise;


	static void init();

	static void updateGbufferResolution(unsigned int width, unsigned int height);

	static void setupGbuffers(unsigned int width, unsigned int height);

	static void gPassDraw(aModel& model, Shader& GPass);
};

#endif // GEOMETRYPASS_CLASS_H
