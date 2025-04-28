#include "TimeUtil.h"
#include <GLFW/glfw3.h>

float TimeUtil::s_DeltaTime = 0.0f;
float TimeUtil::s_lastFrameTime = 0.0f;
float TimeUtil::s_frameRate = 0;
float TimeUtil::s_frameRate1hz = 0;

float TimeAccumulator = 0;
void TimeUtil::updateDeltaTime() {
    float currentFrameTime = static_cast<float>(glfwGetTime());
    s_DeltaTime = currentFrameTime - s_lastFrameTime;
    s_lastFrameTime = currentFrameTime;
    s_frameRate = static_cast<int>(1.0f / s_DeltaTime);
    TimeAccumulator += s_DeltaTime;

    if (TimeAccumulator >= 1.0) {
        s_frameRate1hz = static_cast<int>(1.0f / s_DeltaTime);
        TimeAccumulator = 0;
    }
}