#include "TimeUtil.h"
#include <GLFW/glfw3.h>

float TimeUtil::deltatime = 0.0f;
float TimeUtil::lastFrameTime = 0.0f;
float TimeUtil::frameRate = 0;
float TimeUtil::frameRate1hz = 0;
float TimeUtil::TimeAccumulator= 0;
float TimeUtil::time = 0;
void TimeUtil::update() {
    time = static_cast<float>(glfwGetTime()); // currentFrameTime
    deltatime = time - lastFrameTime;
    lastFrameTime = time;
    frameRate = static_cast<int>(1.0f / deltatime);
    TimeAccumulator += deltatime;

    if (TimeAccumulator >= 1.0) {
        frameRate1hz = static_cast<int>(1.0f / deltatime);
        TimeAccumulator = 0;
    }
}