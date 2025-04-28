#include "UF.h"

void UF::Int(GLuint shader, const char* uniform, int uniInt)
{
    // Set an integer uniform variable for the given shader
    glUniform1i(glGetUniformLocation(shader, uniform), uniInt);
}

void UF::Float(GLuint shader, const char* uniform, GLfloat uniFloat)
{
    glUniform1f(glGetUniformLocation(shader, uniform), uniFloat);
}
void UF::Float2(GLuint shader, const char* uniform, GLfloat uniFloat, GLfloat uniFloat2)
{
    glUniform2f(glGetUniformLocation(shader, uniform), uniFloat, uniFloat2);
}
void UF::Float3(GLuint shader, const char* uniform, GLfloat uniFloat, GLfloat uniFloat2, GLfloat uniFloat3)
{
    glUniform3f(glGetUniformLocation(shader, uniform), uniFloat, uniFloat2, uniFloat3);
}
void UF::Float4(GLuint shader, const char* uniform, GLfloat uniFloat, GLfloat uniFloat2, GLfloat uniFloat3, GLfloat uniFloat4)
{
    glUniform4f(glGetUniformLocation(shader, uniform), uniFloat, uniFloat2, uniFloat3, uniFloat4);
}

void UF::Bool(GLuint shader, const char* uniform, bool uniformBool)
{
	GLint uniformLocation = glGetUniformLocation(shader, uniform);
	glUniform1i(uniformLocation, uniformBool ? 1 : 0);

// std::cout << uniform << " " << uniformBool << std::endl;
}

void UF::DoUniforms(GLuint shader, int doReflections, int doFog) {
	//DO
	Int(shader, "doReflect", doReflections);
	Int(shader, "doFog", doFog);
}
void UF::TrasformUniforms(GLuint shader, GLfloat ConeSI[], GLfloat ConeRot[], glm::vec3 lightPos) {
	//TRANS
	Float3(shader, "InnerLight1", ConeSI[1] - ConeSI[0], ConeSI[1], ConeSI[2]);
	Float3(shader, "spotLightRot", ConeRot[0], ConeRot[1], ConeRot[2]);
	Float3(shader, "lightPos", lightPos.x, lightPos.y, lightPos.z);
}

void UF::Depth(GLuint shader, GLfloat DepthDistance, GLfloat DepthPlane[]) {
	Float(shader, "DepthDistance", DepthDistance);
	Float(shader, "NearPlane", DepthPlane[0]);
	Float(shader, "FarPlane", DepthPlane[1]);
}
void UF::ColourUniforms(GLuint shader, GLfloat fogRGBA[], GLfloat skyRGBA[], GLfloat lightRGBA[], float gamma) {
	//COL
	Float3(shader, "fogColor", fogRGBA[0], fogRGBA[1], fogRGBA[2]);
	Float4(shader, "skyColor", skyRGBA[0], skyRGBA[1], skyRGBA[2], skyRGBA[3]);
	Float4(shader, "lightColor", lightRGBA[0], lightRGBA[1], lightRGBA[2], lightRGBA[3]);
	Float(shader, "gamma", gamma);
}