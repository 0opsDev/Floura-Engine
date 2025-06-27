#ifndef SETTINGS_UTILS_CLASS_H
#define SETTINGS_UTILS_CLASS_H

#include<glad/glad.h>
#include <string>

class SettingsUtils
{
public:
    SettingsUtils();
    static std::string s_WindowTitle; //"OpenGL Window"
    static float s_ViewportVerticies[24];
    static std::string sceneName; // Map loading

    static int tempWidth;
    static int tempHeight;
};

#endif // SETTINGS_UTILS_CLASS_H
