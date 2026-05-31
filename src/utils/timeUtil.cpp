#include "TimeUtil.h"
#include <GLFW/glfw3.h>

float TimeUtil::deltatime = 0.0f;
float TimeUtil::lastFrameTime = 0.0f;
int TimeUtil::frameRate = 0;
int TimeUtil::frameRate1hz = 0;
float TimeUtil::TimeAccumulator= 0;
float TimeUtil::time = 0;
float TimeUtil::priorTime; // for time based effects with motion vectors
void TimeUtil::updateTemplate(float &nDeltatime, float &nLastFrameTime, float &nTime, float &nPriorTime)
{
    nPriorTime = nTime;
    nTime = static_cast<float>(glfwGetTime());
    nDeltatime = nTime - nLastFrameTime;
    nLastFrameTime = nTime;
}

int TimeUtil::frame = 0;

void TimeUtil::update() {
    priorTime = time;
    time = static_cast<float>(glfwGetTime()); // currentFrameTime
    deltatime = time - lastFrameTime;
    lastFrameTime = time;
    frameRate = static_cast<int>(1.0f / deltatime);
    TimeAccumulator += deltatime;
    frame += 1;

    if (TimeAccumulator >= 1.0) {
        frameRate1hz = static_cast<int>(1.0f / deltatime);
        TimeAccumulator = 0;
    }
}

float TimeUtil::mtDeltatime;
float TimeUtil::mtLastFrameTime;
float TimeUtil::mtTime;
float TimeUtil::mtPriorTime; // for time based effects with motion vectors

void TimeUtil::mainThreadUpdate()
{
    updateTemplate(mtDeltatime, mtLastFrameTime, mtTime, mtPriorTime);
}

// physics thread
float TimeUtil::ptDeltatime;
float TimeUtil::ptLastFrameTime;
float TimeUtil::ptTime;
float TimeUtil::ptPriorTime; // for time based effects with motion vectors

void TimeUtil::physicsThreadUpdate()
{
    updateTemplate(ptDeltatime, ptLastFrameTime, ptTime, ptPriorTime);
}

// worker thread
float TimeUtil::wtDeltatime;
float TimeUtil::wtLastFrameTime;
float TimeUtil::wtTime;
float TimeUtil::wtPriorTime; // for time based effects with motion vectors

void TimeUtil::workerThreadUpdate()
{
    updateTemplate(wtDeltatime, wtLastFrameTime, wtTime, wtPriorTime);
}
