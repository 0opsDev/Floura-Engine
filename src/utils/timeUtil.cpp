#include "TimeUtil.h"
#include <GLFW/glfw3.h>

float TimeUtil::deltaTime = 0.0f;
float TimeUtil::lastFrameTime = 0.0f;

void TimeUtil::updateDeltaTime() {
    float currentFrameTime = static_cast<float>(glfwGetTime());
    deltaTime = currentFrameTime - lastFrameTime;
    lastFrameTime = currentFrameTime;
}