#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include<glad/glad.h>
#include"GLFW/glfw3.h"
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>
#include <glm/fwd.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

std::string get_file_contents(const char* filename);

class Shader
{
public:
	GLuint ID;
	void LoadShader(const char* vertexFile, const char* fragmentFile);

	void LoadShaderGeom(const char* vertexFile, const char* fragmentFile, const char* geometryFile);

	void LoadComputeShader(const char* computeFile);

	void Activate();
	void ActivateCompute(int x, int y, int z);
	void Delete();

	void setInt(const char* uniform, int uniforIint);
	void setInt2(const char* uniform, int uniforIint1, int uniforIint2);
	void setInt3(const char* uniform, int uniforIint1, int uniforIint2, int uniforIint3);
	void setInt4(const char* uniform, int uniforIint1, int uniforIint2, int uniforIint3, int uniforIint4);

	void setIntVector(const char* uniform, GLsizei count, const GLint* value);

	void setFloat(const char* uniform, GLfloat uniFloat);
	void setFloat2(const char* uniform, GLfloat uniFloat, GLfloat uniFloat2);
	void setFloat3(const char* uniform, GLfloat uniFloat, GLfloat uniFloat2, GLfloat uniFloat3);
	void setFloat4(const char* uniform, GLfloat uniFloat, GLfloat uniFloat2, GLfloat uniFloat3, GLfloat uniFloat4);
	void setFloatVector(const char* uniform, GLsizei count, const GLfloat* value);
	void setFloat2Vector(const char* uniform, GLsizei count, const GLfloat* value);
	void setFloat3Vector(const char* uniform, GLsizei count, const GLfloat* value);
	void setFloat4Vector(const char* uniform, GLsizei count, const GLfloat* value);

	void setMat3(const char* uniform, glm::mat4 uniformMat3);
	void setMat4(const char* uniform, glm::mat4 uniformMat4);
	void setBool(const char* uniform, bool uniformBool);

private:
	void compileErrors(unsigned int shader, const char* type);
};

#endif
