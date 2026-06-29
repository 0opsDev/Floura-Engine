#ifndef RAYTRACER_CLASS_H
#define RAYTRACER_CLASS_H

#include<iostream>
#include "Render/Object/Texture.h"
#include "camera/Camera.h"
#include <Render/Object/ModelAssimp.h>
#include "Render/Handler/sceneDescription.h"
//#include <gl/GL.h>

class raytracer
{
public:


	static bool RTGlobalTransformFlag;
	static GLuint raytracedOutput;
	static GLuint directSignal;
	static GLuint indirectSignal;
	static GLuint specularSignal;
	static GLuint specularIndirectSignal;
	static GLuint emissionSignal;
	static GLuint NoiseMask;

	static float downscaleFactor;
	static float maxDistance;
	static float noiseThreshold;
	static float reflectionDistance;
	static int reflectionBounces;
	static int indirectBounces;
	static int indirectSamples;
	static int maxAccumulatedFrames;
	static bool doAccumulate;
	static bool resetAccumulationOnDirty;

	static void reloadSceneToRaytracer();

	static void uploadSceneToRaytracer();

	static void clearRaytracerData();

	static void uploadToRaytracer(uint64_t instanceUUID);

	static void removeFromRaytracer(uint64_t instanceUUID);

	static void updateboundingboxes(uint64_t instanceUUID, std::vector<Collision::AABB> rootnodes); // for now root node, soon i will do bvh

	static void modelMatrixUpdate(uint64_t instanceUUID, glm::mat4 newModelMatrix);

	static void uvScaleUpdate(uint64_t instanceUUID, glm::vec2 scale);

	static void initcomputeShader(unsigned int width, unsigned int height);

	static void resizeTexture(unsigned int width, unsigned int height);

	static void render();

private:
};
#endif // RAYTRACER_CLASS_H
