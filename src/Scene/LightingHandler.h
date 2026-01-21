#ifndef LIGHTING_HANDLER_H
#define LIGHTING_HANDLER_H

#include<iostream>
#include<string>
#include<array>
#include <json/json.hpp>
#include <glm/fwd.hpp>
#include <glm/ext/vector_float3.hpp>
#include <Render/Shader/shaderClass.h>
#include <Render/Object/ModelAssimp.h>
#include <Render/Object/Billboard.h>

using json = nlohmann::json;

class LightingHandler
{
public:

	// dir light
	static glm::vec3 dirLightRot;
	static glm::vec3 dirLightPosOut;
	static glm::vec3 directLightCol;
	static float directAmbient;
	static float dirSpecularLight;
	static float DirSMMaxBias;
	static bool doDirLight;
	static bool doDirSpecularLight;

	// dir light
	static glm::mat4 lightProjection;

	static Shader dirShadowMapProgram;
	static Shader dirShadowMapProgramBB;

	static unsigned int shadowMapFBO, shadowMapHeight, shadowMapWidth, dirShadowMap;
	static float distance;
	static glm::vec2 dirNearFar;
	static float dirShadowheight;
	static bool doDirShadowMap;
	static int dirShadowMapHardness;
	static int dirShadowMapSamples;

	struct Light {
		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 colour;
		float radius;
		int type;
		bool enabled;
	};
	static std::vector<Light> Lights;

	static void setupShadowMapBuffer();

	//static void 

	static void drawShadowMap(Model*& model);

	static void drawShadowMapBillboard(BillBoard*& bilboard, glm::vec3 translation, glm::vec3 scale);

	static void update(Shader ModelShader);

	static void createLight();

	static void deleteLight(int index);

	static void loadScene(std::string path);

	static void saveScene(std::string path);

	static void deleteScene();

	static void cleanup();

private:

};
#endif // LIGHTING_HANDLER_H
