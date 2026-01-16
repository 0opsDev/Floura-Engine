#ifndef TEXTURE_MANAGER_CLASS_H
#define TEXTURE_MANAGER_CLASS_H

#include<iostream>
//#include <gl/GL.h>
#include "camera/Camera.h"
#include <map>
#include <unordered_map>

class TextureMapManager
{
public:

	static void uploadHandle(GLuint64 Handle, std::string path); // takes path too

	static void removeHandle(GLuint64 Handle);

	static void removeHandleWpath(std::string path);

	static GLuint64 fetchHandle(std::string path);

	static std::string fetchPath(GLuint64 Handle);

private:
	//					key  | value
	static std::unordered_map<std::string, GLuint64> pKeyHandleMap; // takes path as key, returns handle
	static std::unordered_map<GLuint64, std::string> hKeyPathMap; // takes handle as key, returns path

};
#endif // TEXTURE_MANAGER_CLASS_H
