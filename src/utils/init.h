#ifndef INIT_CLASS_H
#define INIT_CLASS_H

#include <include.h>

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