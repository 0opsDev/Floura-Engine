#ifndef LIGHTINGPASS_CLASS_H
#define LIGHTINGPASS_CLASS_H

#include<iostream>
#include "Render/Object/Texture.h"
#include "camera/Camera.h"
#include <Render/Object/ModelAssimp.h>
//#include <gl/GL.h>

class raytracer
{
public:

	struct triangle
	{
		// vec4 for padding
		glm::vec4 a, b, c;
		// colour
		glm::vec4 aColour, bColour, cColour;
		// texUV
		glm::vec4 aTex;
		glm::vec4 bTex;
		glm::vec4 cTex;
		// normal
		glm::vec4 aNormal, bNormal, cNormal;
		// tangents
		glm::vec4 aTangent, bTangent, cTangent;
		// bitangents
		glm::vec4 aBiTangent, bBiTangent, cBiTangent;
	};

	struct rayMesh // diffuse, specular, normal
	{
		glm::vec4 AABBpos; // 16
		glm::vec4 AABBscale; // 16
		int triangleCount; // 4
		int meshIndex; // 4
		glm::vec2 pad;
	};

	static GLuint computeTexture;
	static GLuint NoiseMask;

	static float downscaleFactor;
	static float maxDistance;
	static float noiseThreshold;
	static float reflectionDistance;
	static int reflectionBounces;
	static bool doAccumulate;
	static bool resetAccumulationOnDirty;
	static Texture* bluenoise;

	static void init();

	static void uploadToRaytracer(Model* model);
	// update and delete functions will be needed (i also need uuids)

	static void initcomputeShader(unsigned int width, unsigned int height);

	static void resizeTexture(unsigned int width, unsigned int height);

	static void render();


private:
	static GLuint triangleSSBOID;
	static GLuint meshSSBOID;
};
#endif // LIGHTINGPASS_CLASS_H
