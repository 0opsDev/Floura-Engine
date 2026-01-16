#include "ShaderHandler.h"
#include "Systems/util/UUID.h"

std::vector<ShaderHandler::shaderObjectS> ShaderHandler::shaderObjects;
std::unordered_map<std::string, uint64_t> ShaderHandler::pKeyHandleMapShader; // takes path as key, returns handle

uint64_t ShaderHandler::fetchHandleRegular(std::string vertexPath, std::string fragPath)
{
	std::string combinedPath = vertexPath + fragPath;
	auto it = pKeyHandleMapShader.find(combinedPath);
	if (it != pKeyHandleMapShader.end()) {
		return it->second;
	}
	return 0;
}

uint64_t ShaderHandler::fetchHandleGeometry(std::string vertexPath, std::string fragPath, std::string geometryPath)
{
	std::string combinedPath = vertexPath + fragPath + geometryPath;
	auto it = pKeyHandleMapShader.find(combinedPath);
	if (it != pKeyHandleMapShader.end()) {
		return it->second;
	}
	return 0;
}

uint64_t ShaderHandler::fetchHandleCompute(std::string computePath)
{
	auto it = pKeyHandleMapShader.find(computePath);
	if (it != pKeyHandleMapShader.end()) {
		return it->second;
	}
	return 0;
}

int ShaderHandler::fetchShaderIndex(uint64_t UUID)
{
	for (size_t i = 0; i < shaderObjects.size(); i++)
	{
		if (shaderObjects[i].UUID == UUID) return (int)i;
	}
	return -1;
}

uint64_t ShaderHandler::createShader(std::string vertexPath, std::string fragPath)
{
	// check if exists
	uint64_t nUUID = fetchHandleRegular(vertexPath, fragPath);
	if (nUUID == 0) // if equal to zero handle does not exist in array, we can create away
	{ 
		// assign new handle
		nUUID = UUID::returnHandle();
		std::string combinedPath = vertexPath + fragPath;
		pKeyHandleMapShader[combinedPath] = nUUID;
		shaderObjectS newSO;
		newSO.UUID = nUUID;
		newSO.Shader.LoadShader(vertexPath.c_str(), fragPath.c_str());
		newSO.instances = 1;
		newSO.type = 0;
		newSO.vertexPath = vertexPath;
		newSO.fragPath = fragPath;
		shaderObjects.push_back(newSO);
	}
	else
	{
		int index = fetchShaderIndex(nUUID);
		if (index != -1)
		{
			shaderObjects[index].instances += 1;
		}
	}

	return nUUID;
}

uint64_t ShaderHandler::createGeometry(std::string vertexPath, std::string fragPath, std::string geometryPath)
{
	// check if exists
	uint64_t nUUID = fetchHandleGeometry(vertexPath, fragPath, geometryPath);
	if (nUUID == 0) // if equal to zero handle does not exist in array, we can create away
	{
		// assign new handle
		nUUID = UUID::returnHandle();
		std::string combinedPath = vertexPath + fragPath + geometryPath;
		pKeyHandleMapShader[combinedPath] = nUUID;

		shaderObjectS newSO;
		newSO.UUID = nUUID;
		newSO.Shader.LoadShaderGeom(vertexPath.c_str(), fragPath.c_str(), geometryPath.c_str());
		newSO.instances = 1;
		newSO.type = 1;
		newSO.vertexPath = vertexPath;
		newSO.fragPath = fragPath;
		newSO.geometryPath = geometryPath;
		shaderObjects.push_back(newSO);
	}
	else
	{
		int index = fetchShaderIndex(nUUID);
		if (index != -1)
		{
			shaderObjects[index].instances += 1;
		}
	}

	return nUUID;
}

uint64_t ShaderHandler::createCompute(std::string computePath)
{
	// check if exists
	uint64_t nUUID = fetchHandleCompute(computePath);
	if (nUUID == 0) // if equal to zero handle does not exist in array, we can create away
	{
		// assign new handle
		nUUID = UUID::returnHandle();
		pKeyHandleMapShader[computePath] = nUUID;

		shaderObjectS newSO;
		newSO.UUID = nUUID;
		newSO.Shader.LoadComputeShader(computePath.c_str());
		newSO.instances = 1;
		newSO.type = 2;
		newSO.computePath = computePath;
		shaderObjects.push_back(newSO);
	}
	else
	{
		int index = fetchShaderIndex(nUUID);
		if (index != -1)
		{
			shaderObjects[index].instances += 1;
		}
	}

	return nUUID;
}

void ShaderHandler::removeInstancewUUID(uint64_t UUID)
{
	int index = fetchShaderIndex(UUID);
	ShaderHandler::removeInstance(index);
}

void ShaderHandler::removeInstance(int index)
{
	// bounds check
	if (index < 0 || index >= (int)shaderObjects.size()) return;

	shaderObjects[index].instances -= 1;

	if (shaderObjects[index].instances <= 0)
	{
		std::string combinedPath;
		if (shaderObjects[index].type == 0)
			combinedPath = shaderObjects[index].vertexPath + shaderObjects[index].fragPath;
		if (shaderObjects[index].type == 1)
			combinedPath = shaderObjects[index].vertexPath + shaderObjects[index].fragPath + shaderObjects[index].geometryPath;
		if (shaderObjects[index].type == 2)
			combinedPath = shaderObjects[index].computePath;

		auto handleIt = pKeyHandleMapShader.find(combinedPath);
		if (handleIt != pKeyHandleMapShader.end()) {
			pKeyHandleMapShader.erase(handleIt);
		}

		shaderObjects[index].Shader.Delete();
		shaderObjects.erase(shaderObjects.begin() + index);
	}
}

void ShaderHandler::reloadShaderwUUID(uint64_t UUID)
{
	int index = fetchShaderIndex(UUID);
	reloadShader(index);
}

void ShaderHandler::reloadShader(int index)
{
	if (index != -1)
	{
		shaderObjects[index].Shader.Delete();

		if (shaderObjects[index].type == 0) // regular
			shaderObjects[index].Shader.LoadShader(shaderObjects[index].vertexPath.c_str(), shaderObjects[index].fragPath.c_str());
		if (shaderObjects[index].type == 1) // geo
			shaderObjects[index].Shader.LoadShaderGeom(shaderObjects[index].vertexPath.c_str(), shaderObjects[index].fragPath.c_str(), shaderObjects[index].geometryPath.c_str());
		if (shaderObjects[index].type == 2) // compute
			shaderObjects[index].Shader.LoadComputeShader(shaderObjects[index].computePath.c_str());
	}

}