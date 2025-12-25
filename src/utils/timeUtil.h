#ifndef TIMEUTIL_H
#define TIMEUTIL_H

class TimeUtil {
public:
    static float deltatime;
    static float lastFrameTime;
    static float frameRate;
    static float frameRate1hz;
    static float TimeAccumulator;
    static void update();
};

#endif // TIMEUTIL_H
