#ifndef UF_CLASS_H
#define UF_CLASS_H

#include <include.h>


class UF
{
public:
    UF();

    void Int(GLuint shader, const char* uniform, int uniInt);
    void Float(GLuint shader, const char* uniform, GLfloat uniFloat);
    void Float2(GLuint shader, const char* uniform, GLfloat uniFloat, GLfloat uniFloat2);
    void Float3(GLuint shader, const char* uniform, GLfloat uniFloat, GLfloat uniFloat2, GLfloat uniFloat3);
    void Float4(GLuint shader, const char* uniform, GLfloat uniFloat, GLfloat uniFloat2, GLfloat uniFloat3, GLfloat uniFloat4);
};

#endif // UF_CLASS_H