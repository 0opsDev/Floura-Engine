#include "TimeUtil.h"
#include <GLFW/glfw3.h>

float TimeUtil::s_DeltaTime = 0.0f;
float TimeUtil::s_lastFrameTime = 0.0f;

void TimeUtil::updateDeltaTime() {
    float currentFrameTime = static_cast<float>(glfwGetTime());
    s_DeltaTime = currentFrameTime - s_lastFrameTime;
    s_lastFrameTime = currentFrameTime;
}