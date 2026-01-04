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
		int modelIndex;
		int mateialIndex;

		uint64_t modelUUID;
		uint64_t meshUUID;
	};

	struct rayModel
	{
		uint64_t modelUUID;
		int ModelIndex;
		int meshCount;
	};

	struct modelHarddata
	{
		rayModel rayModel; // contains the UUID
		std::vector<triangle> tris;
		std::vector<rayMesh> meshes;

		// tracking infomation
		int globalMeshOffset;
		int globalTriangleOffset;
	};

	struct quickRayModel
	{
		uint64_t modelUUID;
		uint64_t padding;
		glm::mat4 ModelMatrix;
		// would be nice to have an active toggle for if do render is toggled
	};

	struct boxRootNode
	{
		glm::vec4 rootPos; // 16
		glm::vec4 rootscale; // 16

		uint64_t modelUUID;
		uint64_t padding;
	};

	struct modelQuickData // for polling updates
	{
		quickRayModel quickModel; // quick model data
		boxRootNode rootNode; // root aabb
	};

	struct modelData // upate variables
	{
		modelHarddata harddata; // full data
		modelQuickData quickdata; // quick update data
	};

	static std::vector<modelData> modelArray;


	static bool RTGlobalTransformFlag;
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

	static void removeFromRaytracer(uint64_t modelUUID);

	static void UpdateModelBuffer();

	static void updateboundingboxes(Model* model); // for now root node, soon i will do bvh

	static void modelMatrixUpdate(uint64_t modelUUID, glm::mat4 newModelMatrix);

	static void updateQuickModelData();

	static void initcomputeShader(unsigned int width, unsigned int height);

	static void resizeTexture(unsigned int width, unsigned int height);

	static void render();


private:
	static GLuint triangleSSBOID;
	static GLuint meshSSBOID;
	static GLuint quickSSBOID;
	static GLuint bvhSSBO;
};
#endif // LIGHTINGPASS_CLASS_H
