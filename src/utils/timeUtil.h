#ifndef TIMEUTIL_H
#define TIMEUTIL_H

class TimeUtil {
public:
    static float deltatime;
    static float lastFrameTime;
    static float time;
    static float priorTime; // for time based effects with motion vectors
    
    static void updateTemplate(float &nDeltatime, float &nLastFrameTime, float &nTime, float &nPriorTime);
    
    static int frameRate;
    static int frameRate1hz;
    static float TimeAccumulator;
    static int frame; // just counts on forever
    static void update();
    
    //main thread
    static float mtDeltatime;
    static float mtLastFrameTime;
    static float mtTime;
    static float mtPriorTime; // for time based effects with motion vectors
    
    static void mainThreadUpdate();
    
    // physics thread
    static float ptDeltatime;
    static float ptLastFrameTime;
    static float ptTime;
    static float ptPriorTime; // for time based effects with motion vectors
    
    static void physicsThreadUpdate();
    
    // worker thread
    static float wtDeltatime;
    static float wtLastFrameTime;
    static float wtTime;
    static float wtPriorTime; // for time based effects with motion vectors
    static void workerThreadUpdate();
};

#endif // TIMEUTIL_H
