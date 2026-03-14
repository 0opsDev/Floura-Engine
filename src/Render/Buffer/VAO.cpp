#include"VAO.h"

VAO::VAO()
{
	glGenVertexArrays(1, &ID);
}

void VAO::LinkAttribI(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset)
{
	VBO.Bind();
	glVertexAttribIPointer(layout, numComponents, type, stride, offset);
	glEnableVertexAttribArray(layout);
	VBO.Unbind();
}

void VAO::LinkAttrib(VBO& VBO, GLuint layout, GLuint numComponets, GLenum type, GLenum normalized, GLsizeiptr stride, void* offset)
{
	VBO.Bind(); // normalized was GL_FALSE
	glVertexAttribPointer(layout, numComponets, type, normalized, stride, offset);
	glEnableVertexAttribArray(layout);
	VBO.Unbind();
}

void VAO::Bind()
{
	glBindVertexArray(ID);
}

void VAO::Unbind()
{
	glBindVertexArray(0);
}

void VAO::Delete()
{
	glDeleteVertexArrays(1, &ID);
}