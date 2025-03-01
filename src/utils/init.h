#ifndef INIT_CLASS_H
#define INIT_CLASS_H

#include"Model.h"
#include <iostream>
#include <vector>
#include "imgui.h"
#include <fstream>
#include <stdexcept>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include"imgui_impl_glfw.h"
#include"imgui_impl_opengl3.h"

class init
{
public:

    init();

    void initGLFW();

    void initImGui(GLFWwindow* window);

    void initBullet();

    void initGLenable(bool frontFaceSide);

};

#endif // INIT_CLASS_H