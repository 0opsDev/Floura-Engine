#ifndef INIT_CLASS_H
#define INIT_CLASS_H

#include <include.h>

class init
{
public:

    static void initGLFW();

    static void initImGui(GLFWwindow* window);

    static void initBullet();

    static void initGLenable(bool frontFaceSide);

    static void initLogo(GLFWwindow* window);

};

#endif // INIT_CLASS_H