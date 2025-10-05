#ifndef LIGHTING_HANDLER_H
#define LIGHTING_HANDLER_H

#include<iostream>
#include<string>
#include<array>
#include <json/json.hpp>
#include <glm/fwd.hpp>
#include <glm/ext/vector_float3.hpp>
#include <Render/Shader/shaderClass.h>

using json = nlohmann::json;

class LightingHandler
{
public:

	// dir light
	static glm::vec3 dirLightRot;
	static glm::vec3 directLightCol;
	static float directAmbient;
	static float dirSpecularLight;
	static bool doDirLight;
	static bool doDirSpecularLight;

	static struct Light {
		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 colour;
		float radius;
		int type;
		bool enabled;
	};

	static std::vector<Light> Lights;

	static void update(Shader ModelShader);

	static void createLight();

	static void loadScene(std::string& path);

	static void saveScene(std::string& path);

	static void deleteScene();

private:

};
#endif // LIGHTING_HANDLER_H
