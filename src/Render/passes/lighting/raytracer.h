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
		int triangleCount; // 4
		int meshIndex; // 4
		int modelIndex;
		int mateialIndex;

		uint64_t instanceUUID;
		uint64_t meshUUID;
		// textures
		uint64_t albedoHandle;
		uint64_t specularHandle;
		uint64_t normalHandle;
	};

	struct rayModel
	{
		uint64_t instanceUUID;
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
		uint64_t instanceUUID;
		uint64_t padding;
		glm::mat4 ModelMatrix;
		glm::mat4 NormalMatrix;
		glm::vec4 uvScale;
		// would be nice to have an active toggle for if do render is toggled
	};

	struct boxRootNode
	{
		glm::vec4 rootPos; // 16
		glm::vec4 rootscale; // 16

		uint64_t instanceUUID;
		uint64_t padding;
	};

	struct modelQuickData // for polling updates
	{
		quickRayModel quickModel; // quick model data
		std::vector<boxRootNode> meshAABBs;
	};

	struct modelData // upate variables
	{
		modelHarddata harddata; // full data
		modelQuickData quickdata; // quick update data
	};

	static std::vector<modelData> modelArray;


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

	static void init();

	static void reloadSceneToRaytracer();

	static void uploadSceneToRaytracer();

	static void clearRaytracerData();

	static void uploadToRaytracer(uint64_t instanceUUID);

	static void removeFromRaytracer(uint64_t instanceUUID);

	static void UpdateModelBuffer();

	static void updateboundingboxes(uint64_t instanceUUID, std::vector<Collision::AABB> rootnodes); // for now root node, soon i will do bvh

	static void modelMatrixUpdate(uint64_t instanceUUID, glm::mat4 newModelMatrix);

	static void uvScaleUpdate(uint64_t instanceUUID, glm::vec2 scale);

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
