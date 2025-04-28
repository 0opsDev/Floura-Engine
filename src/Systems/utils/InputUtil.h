#ifndef INPUT_UTIL_CLASS_H
#define INPUT_UTIL_CLASS_H

#include <iostream>
#include <vector>
#include "Render/Camera.h"
class InputUtil
{
public:

	static std::string CurrentKey;

	static void UpdateCurrentKey(GLFWwindow* window);

};
#endif // INPUT_UTIL_CLASS_H