#ifndef UF_CLASS_H
#define UF_CLASS_H

#include <include.h>


class UF
{
public:
    UF();

    //Data Types
    void Int(GLuint shader, const char* uniform, int uniInt);
    void Float(GLuint shader, const char* uniform, GLfloat uniFloat);
    void Float2(GLuint shader, const char* uniform, GLfloat uniFloat, GLfloat uniFloat2);
    void Float3(GLuint shader, const char* uniform, GLfloat uniFloat, GLfloat uniFloat2, GLfloat uniFloat3);
    void Float4(GLuint shader, const char* uniform, GLfloat uniFloat, GLfloat uniFloat2, GLfloat uniFloat3, GLfloat uniFloat4);

    //Use Of Uniform
    void MassUniforms(GLuint shader);
    void DoUniforms(GLuint shader, int doReflections, int doFog);
    void TrasformUniforms(GLuint shader, GLfloat ConeSI[], GLfloat ConeRot[], glm::vec3 lightPos, GLfloat DepthDistance, GLfloat DepthPlane[]);
    void ColourUniforms(GLuint shader, GLfloat fogRGBA[], GLfloat skyRGBA[], GLfloat lightRGBA[], float gamma);

};

#endif // UF_CLASS_H