#ifndef UF_CLASS_H
#define UF_CLASS_H

#include <include.h>


class UF
{
public:
    //Data Types
    static void Int(GLuint shader, const char* uniform, int uniInt);
    static void Float(GLuint shader, const char* uniform, GLfloat uniFloat);
    static void Float2(GLuint shader, const char* uniform, GLfloat uniFloat, GLfloat uniFloat2);
    static void Float3(GLuint shader, const char* uniform, GLfloat uniFloat, GLfloat uniFloat2, GLfloat uniFloat3);
    static void Float4(GLuint shader, const char* uniform, GLfloat uniFloat, GLfloat uniFloat2, GLfloat uniFloat3, GLfloat uniFloat4);

    //Use Of Uniform
    static void MassUniforms(GLuint shader);
    static void DoUniforms(GLuint shader, int doReflections, int doFog);
    static void TrasformUniforms(GLuint shader, GLfloat ConeSI[], GLfloat ConeRot[], glm::vec3 lightPos);
    static void Depth(GLuint shader, GLfloat DepthDistance, GLfloat DepthPlane[]);
    static void ColourUniforms(GLuint shader, GLfloat fogRGBA[], GLfloat skyRGBA[], GLfloat lightRGBA[], float gamma);

};

#endif // UF_CLASS_H