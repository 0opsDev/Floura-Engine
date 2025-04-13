#ifndef INPUT_UTIL_CLASS_H
#define INPUT_UTIL_CLASS_H

#include <Systems/utils/include.h>


class inputUtil
{
public:

    static float s_sensitivityX;
    static float s_sensitivityY;
    inputUtil();
	static void updateMouse(bool invertMouse[], float sensitivityvar);

    static void updatePitch(bool invertMouseX, float sensitivityvar);

    static void updateYaw(bool invertMouseY, float sensitivityvar);

    static void MouseInput();

};

#endif \