#include "TextureMapManager.h"

std::unordered_map<std::string, GLuint64> TextureMapManager::pKeyHandleMap; // takes path as key, returns handle
std::unordered_map<GLuint64, std::string> TextureMapManager::hKeyPathMap; // takes handle as key, returns path

void TextureMapManager::uploadHandle(GLuint64 Handle, std::string path)
{
	// add to both maps
	pKeyHandleMap[path] = Handle;
	hKeyPathMap[Handle] = path;
}

void TextureMapManager::removeHandle(GLuint64 Handle)
{
	//	remove from both maps
	auto pathIt = hKeyPathMap.find(Handle);
	if (pathIt != hKeyPathMap.end()) {
		std::string path = pathIt->second;
		hKeyPathMap.erase(pathIt);
		pKeyHandleMap.erase(path);
	}
}

void TextureMapManager::removeHandleWpath(std::string path)
{
	//	remove from both maps
	auto handleIt = pKeyHandleMap.find(path);
	if (handleIt != pKeyHandleMap.end()) {
		GLuint64 Handle = handleIt->second;
		pKeyHandleMap.erase(handleIt);
		hKeyPathMap.erase(Handle);
	}
}

GLuint64 TextureMapManager::fetchHandle(std::string path)
{
	auto it = pKeyHandleMap.find(path);
	if (it != pKeyHandleMap.end()) {
		return it->second;
	}
	return 0;
}

std::string TextureMapManager::fetchPath(GLuint64 Handle)
{
	auto it = hKeyPathMap.find(Handle);
	if (it != hKeyPathMap.end()) {
		return it->second;
	}
	return "";
}
