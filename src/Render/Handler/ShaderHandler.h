#ifndef SHADER_HANDLER_CLASS_H
#define SHADER_HANDLER_CLASS_H

#include <iostream>
#include <xhash>
#include <Render/Shader/shaderClass.h>
#include <map>
#include <unordered_map>
#include <vector>

class ShaderHandler
{
public:

	struct shaderObjectS
	{
		uint64_t UUID; // shader uuid
		Shader Shader; // shader object
		int instances; // instances of shader
		int type;
		// paths
		std::string vertexPath;
		std::string fragPath;
		std::string geometryPath;
		std::string computePath;
	};

	static std::vector<shaderObjectS> shaderObjects;

	static uint64_t fetchHandleRegular(std::string vertexPath, std::string fragPath);

	static uint64_t fetchHandleGeometry(std::string vertexPath, std::string fragPath, std::string geometryPath);

	static uint64_t fetchHandleCompute(std::string computePath);

	static int fetchShaderIndex(uint64_t UUID);

	// when we create we also wanna return a handle, so we have these set as handles
	static uint64_t createShader(std::string vertexPath, std::string fragPath);

	static uint64_t createGeometry(std::string vertexPath, std::string fragPath, std::string geometryPath);

	static uint64_t createCompute(std::string computePath);

	static void removeInstancewUUID(uint64_t UUID);

	static void removeInstance(int index);

	static void reloadShaderwUUID(uint64_t UUID);

	static void reloadShader(int index);

private:

	static std::unordered_map<std::string, uint64_t> pKeyHandleMapShader; // takes path as key, returns handle

};
#endif // RENDER_HANDLER_CLASS_H