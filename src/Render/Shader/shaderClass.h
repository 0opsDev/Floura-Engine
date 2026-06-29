#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include <glad/gl.h>
#include"GLFW/glfw3.h"
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>
#include <glm/fwd.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <utils/timeUtil.h>

std::string get_file_contents(const char* filename);

class Shader
{
public:
	GLuint ID;
	bool takePath = true; // if true, tales file path, else takes actual shader code
	bool isSpirv = false;
	void LoadShader(const char* vertexFile, const char* fragmentFile);
	void LoadShaderGeom(const char* vertexFile, const char* fragmentFile, const char* geometryFile);
	void LoadComputeShader(const char* computeFile);

	void Activate();
	void ActivateCompute(int x, int y, int z);
	void Delete();
	
private:
	void compileErrors(unsigned int shader, const char* type);
	
public:
	
	const void setInt(const char* uniform, int uniforIint) {glUniform1i(glGetUniformLocation(ID, uniform), uniforIint);}
	const void setInt(int slot, int uniforIint) {glUniform1i(slot, uniforIint);}
	const void setInt2(const char* uniform, int uniforIint1, int uniforIint2) {glUniform2i(glGetUniformLocation(ID, uniform), uniforIint1, uniforIint2);}
	const void setInt2(int slot, int uniforIint1, int uniforIint2) {glUniform2i(slot, uniforIint1, uniforIint2);}
	const void setInt3(const char* uniform, int uniforIint1, int uniforIint2, int uniforIint3) {glUniform3i(glGetUniformLocation(ID, uniform), uniforIint1, uniforIint2, uniforIint3);}
	const void setInt3(int slot, int uniforIint1, int uniforIint2, int uniforIint3) {glUniform3i(slot, uniforIint1, uniforIint2, uniforIint3);}
	const void setInt4(const char* uniform, int uniforIint1, int uniforIint2, int uniforIint3, int uniforIint4) {glUniform4i(glGetUniformLocation(ID, uniform), uniforIint1, uniforIint2, uniforIint3, uniforIint4);}
	const void setInt4(int slot, int uniforIint1, int uniforIint2, int uniforIint3, int uniforIint4) {glUniform4i(slot, uniforIint1, uniforIint2, uniforIint3, uniforIint4);}
	const void setIntVector(const char* uniform, GLsizei count, const GLint* value) {glUniform1iv(glGetUniformLocation(ID, uniform), count, value);}
	const void setIntVector(int slot, GLsizei count, const GLint* value) {glUniform1iv(slot, count, value);}
	const void setFloat(const char* uniform, GLfloat uniFloat){glUniform1f(glGetUniformLocation(ID, uniform), uniFloat);}
	const void setFloat(int slot, GLfloat uniFloat){glUniform1f(slot, uniFloat);}
	const void setFloat2(const char* uniform, glm::vec2 vector2){glUniform2f(glGetUniformLocation(ID, uniform), vector2.x, vector2.y);}
	const void setFloat2(int slot, glm::vec2 vector2){glUniform2f(slot, vector2.x, vector2.y);}
	const void setFloat3(const char* uniform, glm::vec3 vector3){glUniform3f(glGetUniformLocation(ID, uniform), vector3.x, vector3.y, vector3.z);}
	const void setFloat3(int slot, glm::vec3 vector3){glUniform3f(slot, vector3.x, vector3.y, vector3.z);}
	const void setFloat4(const char* uniform, glm::vec4 vector4){glUniform4f(glGetUniformLocation(ID, uniform), vector4.x, vector4.y, vector4.z, vector4.w);}
	const void setFloat4(int slot, glm::vec4 vector4){glUniform4f(slot, vector4.x, vector4.y, vector4.z, vector4.w);}
	const void setFloatVector(const char* uniform, GLsizei count, const GLfloat* value){glUniform1fv(glGetUniformLocation(ID, uniform), count, value);}
	const void setFloatVector(int slot, GLsizei count, const GLfloat* value){glUniform1fv(slot, count, value);}
	const void setFloat2Vector(const char* uniform, GLsizei count, const GLfloat* value){glUniform2fv(glGetUniformLocation(ID, uniform), count, value);}
	const void setFloat2Vector(int slot, GLsizei count, const GLfloat* value){glUniform2fv(slot, count, value);}
	const void setFloat3Vector(const char* uniform, GLsizei count, const GLfloat* value){glUniform3fv(glGetUniformLocation(ID, uniform), count, value);}
	const void setFloat3Vector(int slot, GLsizei count, const GLfloat* value){glUniform3fv(slot, count, value);}
	const void setFloat4Vector(const char* uniform, GLsizei count, const GLfloat* value){glUniform4fv(glGetUniformLocation(ID, uniform), count, value);}
	const void setFloat4Vector(int slot, GLsizei count, const GLfloat* value){glUniform4fv(slot, count, value);}
	const void setMat3(const char* uniform, glm::mat4 uniformMat3){glUniformMatrix3fv(glGetUniformLocation(ID, uniform), 1, GL_FALSE, glm::value_ptr(uniformMat3));}
	const void setMat3(int slot, glm::mat4 uniformMat3){glUniformMatrix3fv(slot, 1, GL_FALSE, glm::value_ptr(uniformMat3));}
	const void setMat4(const char* uniform, glm::mat4 uniformMat4){glUniformMatrix4fv(glGetUniformLocation(ID, uniform), 1, GL_FALSE, glm::value_ptr(uniformMat4));}
	const void setMat4(int slot, glm::mat4 uniformMat4){glUniformMatrix4fv(slot, 1, GL_FALSE, glm::value_ptr(uniformMat4));}
	const void setBool(const char* uniform, bool uniformBool){glUniform1i(glGetUniformLocation(ID, uniform), uniformBool ? 1 : 0);}
	const void setBool(int slot, bool uniformBool){glUniform1i(slot, uniformBool ? 1 : 0);}
	const void setHandleui64ARB(const char* uniform, GLuint64 handle){glUniformHandleui64ARB(glGetUniformLocation(ID, uniform), handle);}
	const void setHandleui64ARB(int slot, GLuint64 handle){glUniformHandleui64ARB(slot, handle);}
	
	void setTimeVariables() // make slot version
	{
		setFloat("time", TimeUtil::time);
		setFloat("priorTime", TimeUtil::priorTime);
		setFloat("deltatime", TimeUtil::deltatime);
		setInt("frame", TimeUtil::frame);
	}
};

#endif
