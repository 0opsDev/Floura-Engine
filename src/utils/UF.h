#ifndef UF_CLASS_H
#define UF_CLASS_H

#include <iostream>
#include <vector>
#include "Model.h"
#include "Camera.h"
#include "imgui.h"
#include <fstream>
#include <stdexcept>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

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