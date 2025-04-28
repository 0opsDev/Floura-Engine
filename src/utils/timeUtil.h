#ifndef TIMEUTIL_H
#define TIMEUTIL_H

class TimeUtil {
public:
    static float s_DeltaTime;
    static float s_lastFrameTime;
    static float s_frameRate;
    static float s_frameRate1hz;

    static void updateDeltaTime();
};

#endif // TIMEUTIL_H
