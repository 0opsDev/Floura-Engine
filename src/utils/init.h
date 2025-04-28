#ifndef INIT_CLASS_H
#define INIT_CLASS_H

#include <utils/include.h>

class init
{
public:

    static bool LogALL;

    static bool LogModel;

    static bool LogSystems;

    static bool LogLua;

    static void initLog();

    static void initGLFW();

    static void initImGui(GLFWwindow* window);

    static void initBullet();

    static void initGLenable(bool frontFaceSide);

    static void initLogo(GLFWwindow* window);

};

#endif // INIT_CLASS_H