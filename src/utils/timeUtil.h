#ifndef TIMEUTIL_H
#define TIMEUTIL_H

class TimeUtil {
public:
    
    struct timer
    {
        float deltatime;
        float lastFrameTime;
        float time;
        float priorTime;
    };
    
    static void updateTemplate(float &nDeltatime, float &nLastFrameTime, float &nTime, float &nPriorTime);
    
    static float deltatime;
    static float lastFrameTime;
    static float time;
    static float priorTime; // for time based effects with motion vectors
    
    static int frameRate;
    static int frameRate1hz;
    static float TimeAccumulator;
    static int frame; // just counts on forever
    static void update();
    
    //main thread
    static timer mtTimer;
    static void mainThreadUpdate();
    
    // physics thread
    static timer ptTimer;
    static void physicsThreadUpdate();
    
    // worker thread
    static timer wtTimer;
    static void workerThreadUpdate();
};

#endif // TIMEUTIL_H
