#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include<glad/glad.h>
#include<stb/stb_image.h>

#include "Render/Shader/shaderClass.h"

class Texture
{
public:
	GLuint ID;
	std::string type;
	GLuint unit;
	std::string path;
	bool filterNearest = true;
	void createTexture(const char* image, const char* texType, GLuint slot);

	void reload(GLuint slot);

	// Assigns a texture unit to a texture
	void texUnit(Shader& shader, const char* uniform, GLuint unit);
	// Binds a texture
	void Bind();
	// Unbinds a texture
	void Unbind();
	// Deletes a texture
	void Delete();
};
#endif

//texture header is not the problem