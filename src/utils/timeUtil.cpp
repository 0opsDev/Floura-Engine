#include "TimeUtil.h"
#include <GLFW/glfw3.h>

float TimeUtil::deltatime = 0.0f;
float TimeUtil::lastFrameTime = 0.0f;
float TimeUtil::frameRate = 0;
float TimeUtil::frameRate1hz = 0;
float TimeUtil::TimeAccumulator= 0;

void TimeUtil::update() {
    float currentFrameTime = static_cast<float>(glfwGetTime());
    deltatime = currentFrameTime - lastFrameTime;
    lastFrameTime = currentFrameTime;
    frameRate = static_cast<int>(1.0f / deltatime);
    TimeAccumulator += deltatime;

    if (TimeAccumulator >= 1.0) {
        frameRate1hz = static_cast<int>(1.0f / deltatime);
        TimeAccumulator = 0;
    }
}