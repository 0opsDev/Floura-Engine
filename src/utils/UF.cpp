#include "UF.h"

// most useless class ever
UF::UF()
{
}

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