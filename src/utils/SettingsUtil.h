#ifndef SETTINGS_UTILS_CLASS_H
#define SETTINGS_UTILS_CLASS_H

#include <include.h>

class SettingsUtils
{
public:
    SettingsUtils();

    static float ViewportVerticies[24];
    static float skyboxVertices[24]; // Adjust the size as needed
    static unsigned int skyboxIndices[36]; // Adjust the size as needed

    // GLfloat, Render, Camera, Light
    GLfloat ConeSI[3] = { 0.05f, 0.95f , 2.0f };
    GLfloat ConeRot[3] = { 0.0f, -4.0f , 0.0f };
    GLfloat LightTransform1[3] = { 0.0f, 25.0f, 0.0f };
    GLfloat CameraXYZ[3] = { 0.0f, 5.0f, 0.0f };
    GLfloat lightRGBA[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat skyRGBA[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat fogRGBA[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
};

#endif // SETTINGS_UTILS_CLASS_H
