#ifndef INIT_CLASS_H
#define INIT_CLASS_H

#include "imgui/imgui_impl_glfw.h"
#include <string>

class init
{
public:

    static bool LogALL;

    static bool LogModel;

    static bool LogSound;

    static bool LogSystems;

    static bool LogLua;

    static void initLog();

    static void initGLFW();

    static void initImGui(GLFWwindow* window);

    static void initGLenable(bool frontFaceSide);

    static void initLogo(GLFWwindow* window, std::string path);

};

#endif // INIT_CLASS_H