#ifndef GEOMETRYPASS_CLASS_H
#define GEOMETRYPASS_CLASS_H

#include<iostream>
#include <Render/Model/Model.h>

class GeometryPass
{
public:
	static unsigned int depthTexture, gBuffer, gAlbedoSpec, gNormal, gPosition, DBO, gNoise;


	static void init();

	static void updateGbufferResolution(unsigned int width, unsigned int height);

	static void setupGbuffers(unsigned int width, unsigned int height);

	static void gPassDraw(Model& model, glm::vec3 Transform, glm::vec4 Rotation, glm::vec3 Scale);
};

#endif // GEOMETRYPASS_CLASS_H
