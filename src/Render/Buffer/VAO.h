#ifndef VAO_CLASS_H
#define VAO_CLASS_H

#include <glad/gl.h>
#include"VBO.h"

class VAO
{
public:
	GLuint ID;
	VAO();

	void LinkAttribI(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset);
	void LinkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLenum normalized, GLsizeiptr stride, void* offset);
	void Bind();
	void Unbind();
	void Delete();
};
#endif