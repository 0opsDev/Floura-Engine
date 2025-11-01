#ifndef LIGHTING_HANDLER_H
#define LIGHTING_HANDLER_H

#include<iostream>
#include<string>
#include<array>
#include <json/json.hpp>
#include <glm/fwd.hpp>
#include <glm/ext/vector_float3.hpp>
#include <Render/Shader/shaderClass.h>
#include "Scene/IdManager.h"
#include <Render/Object/ModelAssimp.h>

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
	static unsigned int shadowMapFBO, shadowMapHeight, shadowMapWidth, dirShadowMap;
	static float distance;
	static glm::vec2 dirNearFar;
	static float dirShadowheight;
	static bool doDirShadowMap;
	static int dirShadowMapHardness;

	static struct ShadowMaps
	{
		unsigned int shadowMapFBO;
		unsigned int shadowMapHeight;
		unsigned int shadowMapWidth;
		unsigned int ShadowMap;
		unsigned int indexOfLight;
	};

	static int amountPointShadowMaps;
	static int amountPointNear;
	static std::vector<ShadowMaps> PointShadowMap;

	static int amountSpotShadowMaps;
	static int amountSpotNear;
	static std::vector<ShadowMaps> SpotShadowMap;


	static struct Light {
		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 colour;
		float radius;
		int type;
		bool enabled;
		IdManager::ID ID;
		ShadowMaps ShadowMap; // render/bake shadow map for STATIC lights
	};
	static std::vector<Light> Lights;

	static void setupShadowMapBuffer();

	static void nearestLightMapIndexSync(char type, int closeIndex, LightingHandler::ShadowMaps shadowMap);

	static void updateAmountOfLightMaps();

	static void createLightMap(char type);

	static void deleteLightMap(char type, int index);

	static void drawShadowMap(Model& model, glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale);

	static void update(Shader ModelShader);

	static void createLight();

	static void deleteLight(int index);

	static void loadScene(std::string path);

	static void saveScene(std::string path);

	static void deleteScene();

private:

};
#endif // LIGHTING_HANDLER_H
